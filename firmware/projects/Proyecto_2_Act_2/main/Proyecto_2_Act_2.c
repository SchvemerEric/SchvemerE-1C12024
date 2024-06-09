/*! @mainpage proyecto 2 
 *
 * @section genDesc General Description
 *
 *  se desarrolla un sistema capaz de medir distancias utilizando un sensor
 *  ultrasónico y visualizar el resultado de la medición a través de LEDs y un display LCD. 
 *  Además, se implementa la funcionalidad de controlar la medición 
 *  utilizando dos teclas (TEC1 y TEC2) para iniciar/detener la medición 
 *  y mantener el resultado respectivamente.
 * 
 *   punto 2: Se modifica la actividad del punto 1 de manera de utilizar interrupciones
 *  para el control de las teclas y el control de tiempos (Timers). 
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
 * | 10/04/2024 | Comienza el archivo                            |
 * | 8/06/2024 | Finaliza la documentación         		         |
 *
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

/*==================[internal functions declaration]=========================*/
/**
 * @brief Es una tarea destinada a la realización de encender LEDs en función de la distancia sensada además de mostrar 
 * por LCD la distancia medida en el sensor.
*/
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


/**
 * @brief Es un tarea dedicada a realizar las mediciones de distancia con el sensor de ultrasonido
*/
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
	
/**
 * @fn FuncTimerMedir(void *param)
 * @param param parametro que no se utiliza
 * @brief Envía una notificación a la tarea medir
*/
 void FuncTimerMedir(void *param)
{
    vTaskNotifyGiveFromISR(Medir_task_handle, pdFALSE); /* Envía una notificación de la tarea a medir*/
}
/**
 * @fn void FuncTimerMostrar(void *param)
 * @param param parametro que no se utiliza
 * @brief Envía una notificación a la tarea mostrar
*/
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

/*==================[external functions definition]==========================*/
void app_main(void){

LedsInit();
HcSr04Init(GPIO_3, GPIO_2);
SwitchesInit();
LcdItsE0803Init();

    /* timers para medir */
    timer_config_t timer_de_medicion = {
        .timer = TIMER_A,
        .period = REFRESCO_MED,
        .func_p = FuncTimerMedir,// VA EL PARENTESIS? 
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
	
     /*Inicio del conteo de timers*/
    TimerStart(timer_de_medicion.timer);
    TimerStart(timer_de_motrar.timer);
}


/*==================[end of file]============================================*/