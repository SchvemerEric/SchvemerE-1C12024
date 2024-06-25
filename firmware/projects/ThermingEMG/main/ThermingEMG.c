/*! @mainpage TheremingEMG
 *
 * @section genDesc General Description
 *
 * Este proyecto se encarga de levantar una señal de EMG, aplicar unos filtros para poder trabajar claramente
 * con la señal asi como tambien hacer sonar un buffer a partit de los distintos valores de amplitud de la señall
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_X	 	| 	CH1		    |
 * | Pin Buffer     | GPIO23        |
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2023 | Document creation		                         |
 *
 * @author Schvemer Eric
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <math.h> // Incluir la biblioteca para usar fabs

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "switch.h"
#include "gpio_mcu.h"
#include "lcditse0803.h"
#include <hc_sr04.h>
#include "delay_mcu.h"
#include "timer_mcu.h"
#include "uart_mcu.h"
#include "analog_io_mcu.h"
#include "iir_filter.h"
#include "buzzer.h"

/*==================[macros and definitions]=================================*/
/** 
 * @def TaskHandle_t convadc_task_handle
 
 * @brief elemento utilizado para manejar las tareas con interrupciones
*/
TaskHandle_t convadc_task_handle = NULL;
/** 
 * @def TaskHandle_t HacerSonar_task_handle
 
 * @brief elemento utilizado para manejar la tarea de hacerlo sonar
*/
TaskHandle_t HacerSonar_task_handle= NULL;
/**
 * @def BUFFER_SIZE 
 * @brief Tamaño del vector que contiene los datos de un ECG
*/
#define BUFFER_SIZE 2000
uint16_t data;
int i=0; 
/**
 * @def SAMPLE_FREQ 
 * @brief definimos frecuencia maxima 
*/
#define SAMPLE_FREQ	        200
#define T_SENIAL            4000 
/**
 * @def CHUNK 
 * @brief cantidas de muestars que levanta y analiza 
*/
#define CHUNK               4 

#define PWM_BUZZER      PWM_3
#define PWM_DC          50
#define OCTAVE_OFFSET   0


#define BUZZER GPIO_23

/*==================[internal data definition]===============================*/

/** 
 * @def EMG_filt 
 * @brief vector que contiene todos los datos necesario para levantar una señal de EMG
*/
static float EMG_filt[CHUNK];
bool filter = false;

// Definir las notas que se utilizarán
/** 
 * @def notas 
 * @brief Defino las cantidas de notas que utilizara el buffer
*/
int notas[] = {NOTE_C4, NOTE_DS4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_AS4, NOTE_C5, NOTE_DS5}; // Escala blues DO

const int NUM_NOTAS = sizeof(notas) / sizeof(notas[0]);

// Definir umbrales de amplitud
const float THRESHOLDS[] = {0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0}; // Umbrales de amplitud
const int NUM_THRESHOLDS = sizeof(THRESHOLDS) / sizeof(THRESHOLDS[0]);

/*==================[internal functions declaration]=========================*/
uint16_t EMG[CHUNK*2];
/** 
 * @fn  FuncTimer
()
 * @brief Involucrada en enviar una notificación para poder continuar la tarea de conversion AD
*/
void FuncTimer(void *param)
{
    vTaskNotifyGiveFromISR(convadc_task_handle, pdFALSE); /* Envía una notificación de la tarea a medir*/
}


/** 
 * @fn  RectifySignal
()
 * @brief Funsion utulizada para rectificar la señal y quedarnos solo con los valores positivos
*/

void RectifySignal(float* EMG, float* output, size_t length) {
    for (size_t i = 0; i < length; i++) {
        output[i] = fabs(EMG[i]); // Convertir valores negativos a positivos y guardar en output
    }
}

/** 
 * @fn  SonarBuffery_task
()
 * @brief funsion utilizada para hacer sonar el buffer a los distintos valores de frecuencia
*/
void SonarBuffery_task() {
    uint16_t i = 0;  // Declare the variable outside the loop

while (1)
{
//valores procesados de emg filt?? compara valores de amplitud o voltaje
 for (int i = 0; i < CHUNK; i++) {
    if (EMG_filt[i] < 150) {
        BuzzerSetFrec(notas[0]);
    } else if (EMG_filt[i] < 200) {
        BuzzerSetFrec(notas[1]);
    } else if (EMG_filt[i] < 250) {
        BuzzerSetFrec(notas[2]);
    } else if (EMG_filt[i] < 300) {
        BuzzerSetFrec(notas[3]);
    } else if (EMG_filt[i] < 350) {
        BuzzerSetFrec(notas[4]);
    } else if (EMG_filt[i] < 400) {
        BuzzerSetFrec(notas[5]);
    } else if (EMG_filt[i] < 450) {
        BuzzerSetFrec(notas[6]);
    } else if(EMG_filt[i]<500) {
        BuzzerSetFrec(notas[7]);
    }else {}

    BuzzerOn();
    vTaskDelay(1000/ portTICK_PERIOD_MS);
    }
}}

void FuncTimerSenial(void* param){//notifico la tarea
    xTaskNotifyGive(convadc_task_handle);

}
/** 
 * @fn  Filtrar_EMG_task
()
 * @brief Funsion utilizada para aplicar los filtros y tomar la señal de EMG 
*/
static void Filtrar_EMG_task(void *pvParameter){
    char msg[128];
    char msg_chunk[24];
    static uint8_t indice = 0;

    while(true){

		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);   
		AnalogInputReadSingle(CH1, &EMG[indice]);
       indice++;

        //printf("%d\n", indice);
        if(indice >= CHUNK){
            
            for(uint8_t i = 0; i < CHUNK; i++){
                EMG_filt[i] = EMG[i];//le asigno dato flotante 
            }
            HiPassFilter(EMG_filt, EMG_filt, CHUNK);
			RectifySignal(EMG_filt,EMG_filt,CHUNK);
            LowPassFilter(EMG_filt, EMG_filt, CHUNK);
    
            strcpy(msg, "");
            for(uint8_t i = 0; i < CHUNK; i++){
                //CheckAndSetFrequency(EMG_filt[i]);  // Verificar la amplitud y establecer la frecuencia
                sprintf(msg_chunk, "%.2f\r", EMG_filt[i]);
                strcat(msg, msg_chunk);
            }
            UartSendString(UART_PC, msg);
            indice = 0;
        } else{
        } 
        // Comprobar la amplitud y ajustar la frecuencia
        
    
    }
}
/** 
 * @fn  DetermineNoteIndex
()
 * @brief Función para determinar la nota basada en la amplitud
*/

int DetermineNoteIndex(float amplitude) {
    for (int i = 0; i < NUM_THRESHOLDS; i++) {
        if (amplitude < THRESHOLDS[i]) {
            return i;
        }
    }
    return NUM_THRESHOLDS - 1; // Nota más alta si supera todos los umbrales
}

/*==================[external functions definition]==========================*/
void app_main(void){

BuzzerInit(BUZZER);

HiPassInit(SAMPLE_FREQ, 0.1, ORDER_2);
LowPassInit(SAMPLE_FREQ, 5, ORDER_2);

analog_input_config_t analog = {
		.input = CH1, //le paso el canal
		.mode = ADC_SINGLE,
		.func_p = NULL,
		.param_p = NULL,
		.sample_frec = 0
};
AnalogInputInit(&analog); //inicializo

timer_config_t timer_1 = {
			.timer = TIMER_A,
			.period = 2000,
			.func_p = FuncTimer,
			.param_p = NULL
};
TimerInit(&timer_1);

//podes ponerle una opcion que te cambie la escala de notas
serial_config_t my_uart = {
		.port = UART_PC, 
		.baud_rate = 115200, //ver este tiempo 
		.func_p = NULL, 
		.param_p = NULL
};
UartInit(&my_uart);

AnalogOutputInit(); //ini la salida

xTaskCreate(&Filtrar_EMG_task, "Filtros", 8000, NULL, 4, &convadc_task_handle);
xTaskCreate(&SonarBuffery_task, "buffer", 8000, NULL, 5, &HacerSonar_task_handle);
TimerStart(timer_1.timer);


}
/*==================[end of file]============================================*/