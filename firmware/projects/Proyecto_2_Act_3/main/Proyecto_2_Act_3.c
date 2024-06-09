/*! @mainpage proyecto 2 
 *
 * @section genDesc General Description
 *
 * Esta aplicacion permite la detección de distancias mediante un sensor de ultrasonido, encendiendo leds para 
 * ciertos rangos de distancias y mostrando dicha distancia en una pantalla LCD mediante la utilizacion de tareas 
 * controladas por timers e interrupciones. Además de informar por conexión serie la distancia medida por el sensor..
 * 
 *
 * @section hardConn Hardware Connection
*  | Peripheral | ESP32                                           |
 * |:----------:|:-----------------------------------------------|
 * | lcditse0803|            		                             |
 * | HC-SR04    | GPIO_3                                         |
 *
 *
 * @section changelog Changelog
	*
	* |   Date	    | Description                                    |
	* |:----------:|:-----------------------------------------------|
	* | 15/04/2024 | Inicio del proyecto		                     |
	* | 8/06/2024 | Fin de documentacion del proyecto              |
 *
 * @author schvemer Eric 
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
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


/*==================[macros and definitions]=================================*/
/** @def REFRESCO_MED
 * @brief Representa el tiempo en microsegundos que estará comandando el disparo del timer 
 * que activa la tarea de medir
*/
#define REFRESCO_MED 1000000
/** @def REFRESCO_LCD
 * @brief Representa el tiempo en microsegundos que estará comandando el disparo del timer 
 * que activa la tarea de mostrar por display y control de Leds
*/
#define REFRESCO_LCD 100000
/*==================[internal data definition]===============================*/
bool on; 
bool off; 
int distancia; 

bool start= false;  /*/uso para activar y desactivar la medicion*/ //puedo usar un star t1 y hold t2 //STARTR
bool hold= false ;  /*uso para mantener el estado de HOLD*///HOLD
/**
 * @def Medir_task_handle 
 * @brief Objeto para el manejo de tarea medir
*/
TaskHandle_t Medir_task_handle = NULL; //TaskHandle_t se utilizará para referirse a punteros a bloques de control de tarea.
/**
 * @def Mostrar_task_handle
 * @brief Objeto para el manejo de tarea mostrar
 * 
*/
TaskHandle_t Mostrar_task_handle = NULL;

TaskHandle_t mostrar_task_handle = NULL;

TaskHandle_t Teclas_task_handle = NULL;
TaskHandle_t leds_task_handle = NULL;
TaskHandle_t display_task_handle = NULL;
/*==================[internal functions declaration]=========================*/

void MostrarDistancias_task(void *pvParameter){
			uint16_t distancia=0; 

	while (1)
	{
         ulTaskNotifyTake(pdTRUE, portMAX_DELAY); //espero esto hasta recibir notifc

		if (start)
		{

			distancia = HcSr04ReadDistanceInCentimeters();//mide distancia en cm 
 			if (!hold)
			{								 // si no apreto la tecla2
				LcdItsE0803Write(distancia); // pantalla LCD
			}
			if (distancia < 10)
			{
				LedsOffAll();
			}

			else if (distancia > 10 && distancia < 20)
			{
				LedOn(LED_1);
				LedOff(LED_2);
				LedOff(LED_3);
			}

			else if (distancia > 20 && distancia < 30)
			{
				LedOn(LED_1);
				LedOn(LED_2);
				LedOff(LED_3);
			}

			else if (distancia > 30)
			{
				LedOn(LED_1);
				LedOn(LED_2);
				LedOn(LED_3);
			}

           
		}
		else
		{
			LcdItsE0803Off();
			LedsOffAll();
		}

		//vTaskDelay(CONFIG_BLINK_PERIOD_1/ portTICK_PERIOD_MS)
	}
}



    static void medir_dist(void *pvParameter)
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /*la tarea espera en este punto hasta recibir la notificacion*/
        if (start)
        {
            distancia = HcSr04ReadDistanceInCentimeters();
        }
    }
}
	
 void FuncTimerMedir(void *param)
{
    vTaskNotifyGiveFromISR(Medir_task_handle, pdFALSE); /* Envía una notificación de la tarea a medir*/
}

void FuncTimerMostrar(void *param)
{
    vTaskNotifyGiveFromISR(Mostrar_task_handle, pdFALSE); /* Envía una notificación de la tarea a mostrar */
}

void Tstart() 
{
    start = !start;
}

void THold()
{
    if (start)
    {
        hold = !hold;
    }
}
static void DisplayTask(void *pvParameter){
	while(true){
    	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);   
		if(on == true){
			if(hold == false){
				LcdItsE0803Write(distancia);
			}
		}
		else{
			LcdItsE0803Off();
		} 
	}
}
//modificacion del proyecto tres 
/**
 * @fn Mostrar_Distancia_PC()
 * @brief Permite mostrar por monitor serial la distancia sensada en tiempo real
*/
void Mostrar_Distancia_PC(){
UartSendString(UART_PC, "distancia"); 
  UartSendString(UART_PC, (char *)UartItoa(distancia, 10));//* @brief Convert a number to a String (char array ended with '\0')
    UartSendString(UART_PC, " cm\r\n");

}
/**
 * @fn TSerieOnOff(){
()
 * @brief Cambia el estado de las banderas booleanas On y Hold en función de la entrada por conexión de puerto
 * serie
*/
 void TSerieOnOff(){
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
 static void MostrarTask(void *pvParameter){
	while(true){
    	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);   
		if(on == true){
			if(hold == false){
				Mostrar_Distancia_PC();
 			}
		}
		else{
		} 
	}
}
/*==================[external functions definition]==========================*/
void app_main(void){

LedsInit();
HcSr04Init(GPIO_3, GPIO_2);
SwitchesInit();
LcdItsE0803Init();

serial_config_t my_uart = {
		.port = UART_PC, 
		.baud_rate = 9600, 
		.func_p = &TSerieOnOff, 
		.param_p = NULL
	};
	UartInit(&my_uart);

    /* timers para medir */
    timer_config_t timer_de_medicion = {
        .timer = TIMER_A,
        .period = REFRESCO_MED,
        .func_p = FuncTimerMedir, 
        .param_p = NULL
    };  TimerInit(&timer_de_medicion);

    /* timer para mostrar */
    timer_config_t timer_de_motrar = {
        .timer = TIMER_B,
        .period = REFRESCO_LCD,
        .func_p = FuncTimerMostrar,
        .param_p = NULL
    }; TimerInit(&timer_de_motrar);

    /*En este caso, a partir de crear la interrupcion iniciada por el SWITCH_1 elijo que la funcion estadoTeclas
    se va a ejecutar*/
    SwitchActivInt(SWITCH_1, &Tstart, NULL);

    /*En este caso, a partir de crear la interrupcion iniciada por el SWITCH_2 elijo que la funcion estadoTeclas
    se va a ejecutar*/
    SwitchActivInt(SWITCH_2, &THold, NULL);

    /*creacion de tareas*/
    xTaskCreate(&medir_dist, "medir", 512, NULL, 5, &Medir_task_handle);
    xTaskCreate(&MostrarDistancias_task, "mostrar", 512, NULL, 5, &Mostrar_task_handle);
	xTaskCreate(&DisplayTask, "Display", 512, NULL, 4, &display_task_handle);
	xTaskCreate(&MostrarTask, "Mostrar", 512, NULL, 4, &mostrar_task_handle);

	//xTaskCreate(&distancias_task, "distancias_task", 512, NULL, 5, NULL);
	//xTaskCreate(&switchTeclas_task, "switchTeclas_task", 512, NULL, 5, NULL);

     /*Inicio del conteo de timers*/	
    TimerStart(timer_de_medicion.timer);
    TimerStart(timer_de_motrar.timer);
}


/*==================[end of file]============================================*/