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
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "switch.h"
#include "gpio_mcu.h"
#include "lcditse0803.h"
#include <hc_sr04.h>
#include "delay_mcu.h"
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/
bool on; 
bool off; 
uint16_t d; 

bool tecla_1; 
bool tecla_2; 

#define CONFIG_BLINK_PERIOD 1000

/*==================[internal functions declaration]=========================*/

void distancias_task(void *pvParameter){
	while (1)
	{
		uint16_t distancia; 
		distancia= HcSr04ReadDistanceInCentimeters(); 
		LcdItsE0803Write(distancia); //aca llamamos a la pantalla del lcd

		if (distancia < 10)
		{
			LedsOffAll(); 
		}
		else if (distancia>10 && distancia<20)
		{
			LedOn(LED_1);
			LedOff(LED_2);
			LedOff(LED_3);
		}
		else if (distancia>20 && distancia<30)
		{
			
			LedOn(LED_1);
			LedOn(LED_2);
			LedOff(LED_3);
		}
		else if (distancia>30)
		{
			LedOn(LED_1);
			LedOn(LED_2);
			LedOn(LED_3);
		}
        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
	}
	
}

void switchTeclas_task(void* PvParameter){
	uint8_t teclas; 
	while (1)
	{
		teclas=SwitchesRead(); 
		switch(teclas){
			case SWITCH_1: 
			tecla_1 true; 
		break; 
			case SWITCH_2: 
			tecla_2 true; 
		break; 
	
		}
	vTaskDelay(CONFIG_BLINK_PERIOD/portTICK_PERIOD_MS)

	}
	
}

/*==================[external functions definition]==========================*/
void app_main(void){
LedsInit();
LcdItsE0803Init(); 
HcSr04Deinit(); 
SwitchesInit(); 
xTaskCreate(& distancias_task, "distancias_task", 512, NULL,5,NULL);
xTaskCreate(& switchTeclas_task, "switchTeclas_task", 512, NULL,5,NULL);
}

}
/*==================[end of file]============================================*/