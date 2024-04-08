/*! @mainpage Blinking switch
 *
 * \section genDesc General Description
 *
 * This example makes LED_1 and LED_2 blink if SWITCH_1 or SWITCH_2 are pressed.
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
/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD 1000
//#define ON 1
//#define OFF 0
//#define TOGGLE
/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/
void app_main(void){
	uint8_t teclas;
	//bool switch1_presionado = false;
    //bool switch2_presionado = false;
	LedsInit();
	SwitchesInit();
    while(1)    {
    	 teclas = SwitchesRead();
        switch1_presionado = (teclas & SWITCH_1) ? true : false;
        switch2_presionado = (teclas & SWITCH_2) ? true : false;

        if (switch1_presionado && switch2_presionado) {
            LedToggle(LED_1);
            LedToggle(LED_2);
        } else {
            // Si no se presionan simultáneamente las teclas 1 y 2, apagar los LEDs 1 y 2
            LedOff(LED_1);
            LedOff(LED_2);
    	}
	    LedToggle(LED_3);
		vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
	}
}

/*while (1) {
        teclas = SwitchesRead();
        switch1_presionado = (teclas & SWITCH_1) ? true : false;
        switch2_presionado = (teclas & SWITCH_2) ? true : false;

        if (switch1_presionado && switch2_presionado) {
            LedToggle(LED_1);
            LedToggle(LED_2);
        } else {
            // Si no se presionan simultáneamente las teclas 1 y 2, apagar los LEDs 1 y 2
            LedOff(LED_1);
            LedOff(LED_2);
        }

        vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
    }*/

	/*

// Definición de la estructura
struct leds {
    uint8_t mode;      // ON, OFF, TOGGLE
    uint8_t n_led;     // Número del LED a controlar
    uint8_t n_ciclos;  // Cantidad de ciclos de encendido/apagado
    uint16_t periodo;  // Tiempo de cada ciclo
};

// Función para controlar el LED
void controlarLED(struct leds *led) {
    // Verificar el modo del LED
    switch (led->mode) { //puntero a la estrucutra 
        case ON:
            // Encender el LED
            // Implementación específica para encender el LED con número led->n_led

            break;
        case OFF:
            // Apagar el LED
            // Implementación específica para apagar el LED con número led->n_led

            break;
        case TOGGLE:
            // Alternar el estado del LED
            // Implementación específica para alternar el estado del LED con número led->n_led

            break;
        default:
            // Modo no válido, hacer algo en consecuencia
            break;
    }
}
*/