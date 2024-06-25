/*! @mainpage Blinking
 *
 * \section genDesc General Description
 *
 * This example makes LED_1, LED_2 and LED_3 blink at different rates, using FreeRTOS tasks.
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
/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD_LED_1 1000
#define CONFIG_BLINK_PERIOD_LED_2 1500
#define CONFIG_BLINK_PERIOD_LED_3 500
/*==================[internal data definition]===============================*/
TaskHandle_t led1_task_handle = NULL;
TaskHandle_t led2_task_handle = NULL;
TaskHandle_t led3_task_handle = NULL;
/*==================[internal functions declaration]=========================*/
static void Led1Task(void *pvParameter){
    while(true){
        printf("LED_1 ON\n");
        LedOn(LED_1);/*a parrir de la def de modulos*/
        vTaskDelay(CONFIG_BLINK_PERIOD_LED_1 / portTICK_PERIOD_MS);/*kerner del frtos vtaskdelay es una que me permite controloar el micro*/
        printf("LED_1 OFF\n");
        LedOff(LED_1);
        vTaskDelay(CONFIG_BLINK_PERIOD_LED_1 / portTICK_PERIOD_MS);
    }
}

static void Led2Task(void *pvParameter){
    while(true){
        printf("LED_2 ON\n");
        LedOn(LED_2);
        vTaskDelay(CONFIG_BLINK_PERIOD_LED_2 / portTICK_PERIOD_MS);
        printf("LED_2 OFF\n");
        LedOff(LED_2);
        vTaskDelay(CONFIG_BLINK_PERIOD_LED_2 / portTICK_PERIOD_MS);
    }
}

static void Led3Task(void *pvParameter){
    while(true){
        printf("LED_3 ON\n");
        LedOn(LED_3);
        vTaskDelay(CONFIG_BLINK_PERIOD_LED_3 / portTICK_PERIOD_MS);
        printf("LED_3 OFF\n");
        LedOff(LED_3);
        vTaskDelay(CONFIG_BLINK_PERIOD_LED_3 / portTICK_PERIOD_MS);
    }
}
/*==================[external functions definition]==========================*/ /*este es la entrada*/
void app_main(void){ /*es uana funsion que crea tarea, tarea que llama tarea */
    LedsInit();
    xTaskCreate(&Led1Task, "LED_1", 512, NULL, 5, &led1_task_handle);
    xTaskCreate(&Led2Task, "LED_2", 512, NULL, 5, &led2_task_handle);
    xTaskCreate(&Led3Task, "LED_3", 512, NULL, 5, &led3_task_handle);
}
/*que hago para ccrear la tara: la paametrizo, el nombre de la tarea es led 1 luego el 
512es un tañano luego tenemos un campo para parsarle parametros que el el Null como no quiero nada 
y despues ta la prioridad de la tarea(vamos a usar la coperativa, que va a antrar la que tenga la rpioridad mas 
alta)*/

