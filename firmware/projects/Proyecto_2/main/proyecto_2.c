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
 * 
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_X	 	| 	GPIO_X		|
 *
 *
 * @section changelog Changelog
  * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 10/04/2024 | Comienza el archivo                            |
 * | 8/06/2024 | Finaliza la documentación         |
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
/*==================[macros and definitions]=================================*/
/** @def REFRESCO_MEDICION
 * @brief Representa el tiempo en microsegundos que se utilizará para dar un delay a la tarea mosttrar distancia
 * 
*/
#define CONFIG_BLINK_PERIOD_1 1000

/** @def CONFIG_BLINK_PERIOD_2
 * @brief Representa el tiempo en microsegundos que se utilizará para dar un delay a la tarea switchTeclas_task
 * 
*/
//switchTeclas_task

#define CONFIG_BLINK_PERIOD_2 50

/*==================[internal data definition]===============================*/
/**
 * @def on
 * @brief Variable global de tipo booleana que almacena el estado de encendido del sistema de medición
*/
bool on; 
/**
 * @def off
 * @brief Variable global de tipo booleana que almacena el estado de apagado del sistema de medición
*/
bool off; 

bool tecla_1= false;  /*/uso para activar y desactivar la medicion*/ //puedo usar un star t1 y hold t2 
bool tecla_2= false ;  /*uso para mantener el estado de HOLD*/
/**
 * @def distancia
 * @brief Variable global entera sin signo que almacena la distancia medida por el sensor de ultrasonido
*/
uint16_t distancia; 
/*==================[internal functions declaration]=========================*/
/**
 * @brief Tarea que permite realizar las mediciones  y mostrar la distancia con el sensor de ultrasonido
*/

void MostrarDistancias_task(void *pvParameter){
			uint16_t distancia=0; 

	while (1)
	{
		
		if (tecla_1)
		{

			distancia = HcSr04ReadDistanceInCentimeters();
			if (!tecla_2)
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

		vTaskDelay(CONFIG_BLINK_PERIOD_1/ portTICK_PERIOD_MS);
	}
}

/**
 * @brief Tarea que permite realizar el control de la aplicacion mediante las teclas que se detecten como pulsadas
*/

void switchTeclas_task(void* PvParameter){
	uint8_t teclas; 
	while (1)
	{
		teclas=SwitchesRead(); 
		switch(teclas){
			case SWITCH_1: 
			// TEC1 para activar y detener la medición
			tecla_1 = !tecla_1;
			break;
			case SWITCH_2:
			// TEC2 para mantener el resultado (“HOLD”)
			tecla_2 = !tecla_2;
			break;
	
		}
	vTaskDelay(CONFIG_BLINK_PERIOD_2/portTICK_PERIOD_MS);

	}
	
}

/*==================[external functions definition]==========================*/
void app_main(void){

	HcSr04Init(GPIO_3, GPIO_2);
	LedsInit();
	SwitchesInit();
	LcdItsE0803Init();

	xTaskCreate(&MostrarDistancias_task, "MostrarDistancias_task", 512, NULL, 5, NULL);
	xTaskCreate(&switchTeclas_task, "switchTeclas_task", 512, NULL, 5, NULL);

}


/*==================[end of file]============================================*/