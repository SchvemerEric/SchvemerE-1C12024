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
#include <led.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
//#include "led_pointer.h"


/*==================[macros and definitions]=================================*/
#define ON 1
#define OFF 0 //le tengo que asignar valores ditintos 
#define TOGGLE 2
#define CONFIG_BLINK_PERIOD 100
//#define retardo 500 // ver esto con la placa.  
/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/
	// Definición de la estructura
struct leds {
    uint8_t mode;      // ON, OFF, TOGGLE
    uint8_t n_led;     // Número del LED a controlar
    uint8_t n_ciclos;  // Cantidad de ciclos de encendido/apagado
    uint16_t periodo;  // Tiempo de cada ciclo
} my_leds; 



// Función para controlar el LED
void controlarLED(struct leds *led) {
    // Verificar el modo del LED
    switch (led->mode) { //puntero a la estrucutra 
        case ON:
            // Encender el LED
            // Implementación específica para encender el LED con número led->n_led
            // Encender el LED correspondiente según el número de LED
            if (led->n_led == 1) {
                LedOn(LED_1);
            } else if (led->n_led == 2) {
                LedOn (LED_2);
            } else if (led->n_led == 3) {
                LedOn (LED_3);
            } else {
                printf("Número de LED no válido: %d\n", led->n_led);
            }
            break;
        case OFF:
            // Apagar el LED
            // Implementación específica para apagar el LED con número led->n_led
            if (led->n_led == 1) {
                LedOff(LED_1);
            } else if (led->n_led == 1) {
                LedOff(LED_2);
			 } else if (led->n_led == 1) {
               LedOff(LED_3);
            } //else {
                //printf("Número de LED no válido: %d\n", led->n_led);
				 //toggleLED(led->n_led);
           // }
            break;
        case TOGGLE:
            // Alternar el estado del LED
            // Implementación específica para alternar el estado del LED con número led->n_led
           for(int i=0; i<led->n_ciclos; i++) {
                printf("for i\n");
				switch(led->n_led) 	{
                    case 1:
                        LedToggle(LED_1);

                        break;
                    case 2:
                        LedToggle(LED_2);

                        break;
                    case 3:
                        LedToggle(LED_3);

                        break;
                }
                for(int i=0; i<led->periodo; i++) {
                    vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
                    printf("for j\n");
                }
                
           }
        break;
			
        default:
           printf("fin");
        break;
    }
}




	/* initializations */
void app_main(void){

	LedsInit();
	my_leds.n_ciclos = 10;
	my_leds.periodo = 5;
	my_leds.n_led=1;
	my_leds.mode= TOGGLE; 
	controlarLED(& my_leds);

}


	

/*==================[end of file]============================================*/