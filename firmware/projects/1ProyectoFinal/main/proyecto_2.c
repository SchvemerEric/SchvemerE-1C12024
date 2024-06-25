/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_X	 	| 	GPIO_X		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2023 | Document creation		                         |
 *
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
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
#define CONFIG_BLINK_PERIOD 2000
#define CONFIG_BLINK_PERIODo 4329 //1/235(muestras)
#define CONFIG_BLINK_PERIOD 2000

#define CONFIG_BLINK_PERIOD 500
#define BUFFER_SIZE         256
#define SAMPLE_FREQ	        200
#define T_SENIAL            4000 
#define CHUNK               4 

#define PWM_BUZZER      PWM_3
#define PWM_DC          50
#define OCTAVE_OFFSET   0
#define BUFFER_SIZE 2000
/************************/
#define BUZZER
uint16_t data;
int i=0; 

TaskHandle_t convadc_task_handle = NULL;

TaskHandle_t Buffer = NULL;


/*==================[internal data definition]===============================*/




//int notas_La[]={NOTE_A4,NOTE_B4,NOTE_C5,NOTE_D5,NOTE_E5,NOTE_F5,NOTE_G5,NOTE_A5}; // Escala LA menor

static float EMG_filt[CHUNK];
bool filter = false;

// Definir las notas que se utilizarán
int notas[] = {NOTE_C4, NOTE_DS4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_AS4, NOTE_C5, NOTE_DS5}; // Escala blues DO

const int NUM_NOTAS = sizeof(notas) / sizeof(notas[0]);

// Definir umbrales de amplitud
const float THRESHOLDS[] = {0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0}; // Umbrales de amplitud
const int NUM_THRESHOLDS = sizeof(THRESHOLDS) / sizeof(THRESHOLDS[0]);
uint16_t EMG[CHUNK*2];

/*==================[internal functions declaration]=========================*/

  // Función para determinar la nota basada en la amplitud
int DetermineNoteIndex(float amplitude) {
    for (int i = 0; i < NUM_THRESHOLDS; i++) {
        if (amplitude < THRESHOLDS[i]) {
            return i;
        }
    }
    return NUM_THRESHOLDS - 1; // Nota más alta si supera todos los umbrales
}
void CheckAndSetFrequency(float amplitude) {
    if (amplitude < 0.5) {
        BuzzerSetFrec(notas[0]);
    } else if (amplitude < 1.0) {
        BuzzerSetFrec(notas[1]);
    } else if (amplitude < 1.5) {
        BuzzerSetFrec(notas[2]);
    } else if (amplitude < 2.0) {
        BuzzerSetFrec(notas[3]);
    } else if (amplitude < 2.5) {
        BuzzerSetFrec(notas[4]);
    } else if (amplitude < 3.0) {
        BuzzerSetFrec(notas[5]);
    } else if (amplitude < 3.5) {
        BuzzerSetFrec(notas[6]);
    } else {
        BuzzerSetFrec(notas[7]);
    }
}

/*void TSerieOnOff(){
	uint8_t pres; 
	  UartReadByte(UART_PC, &pres);
    switch (pres)
    {
    case 'O':
        on = !on;
        break;

    case 'H':
        hold = !hold;
        break;
    }

 }
*/

void buffer_control(){


}

void RectifySignal(float* EMG, float* output, size_t length) {
    for (size_t i = 0; i < length; i++) {
        output[i] = fabs(EMG[i]); // Convertir valores negativos a positivos y guardar en output
    }
}

static void FftTask(void *pvParameter){
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

void FuncTimer(void *param)
{
    vTaskNotifyGiveFromISR(convadc_task_handle, pdFALSE); /* Envía una notificación de la tarea a medir*/
}

/*==================[external functions definition]==========================*/
void app_main(void){



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

xTaskCreate(&FftTask, "FFT", 8000, NULL, 4, &convadc_task_handle);

xTaslCreate(&Buffer_control, "Energia", 8000, NULL, $Buffer_task_handle); 

TimerStart(timer_1.timer);


}
/*==================[end of file]============================================*/