/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 *este proyecto se encarga de mantener la humedad y la ph de una planta, controlando estos valores 
 * e infromando estos valores por uart 
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * |  Sensor Humedad | 	GPIO_2		|
 * |  Sensor pH 	 | 	GPIO_34		|
 * |  Bomba Agua	 | 	GPIO_15		|
 * |  Bomba Básica   | 	GPIO_16		|
 * |  Bomba Ácida    | 	GPIO_17		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 11/06/2024 | Document creation		                         |
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
TaskHandle_t HUMEDAD = NULL;
TaskHandle_t P_h = NULL;
#define REFRESCO_Humedad 3000000
#define REFRESCO_Ph 3000000

/** 
 * @def phMin
 
 * @brief definimos el ph minimo
*/
const float phMin = 6.0;
/** 
 * @def phMax
 
 * @brief definimos el pf maximo
*/
const float phMax = 6.7;

/** 
 * @def ccontroldehumedad
 
 * @brief elemento utilizado paracontrolar la humedad de la planta
*/
TaskHandle_t ccontroldehumedad = NULL;
/** 
 * @def controldeph
 * @brief elemento utilizado para controlar el ph de la planta
*/
TaskHandle_t controldeph = NULL;

/*==================[internal data definition]===============================*/
bool on; 
bool off; 

bool start= false;  /*/uso para activar y desactivar la medicion*/ //puedo usar un star t1 y hold t2 //STARTR
bool hold= false ;  /*uso para mantener el estado de HOLD*///HOLD
/**
 * @brief Estructura del puerto GPIO, con un número de pin y una dirección
 */
typedef struct
{
	gpio_t pin;  /*!< GPIO pin number */
	io_t dir;	// GPIO direction '0' IN;  '1' OUT*/

} gpioConf_t;

gpioConf_t gpio[5]={
	{GPIO_19, GPIO_OUTPUT},//PIN_SENSOR_HUMEDAD
	{GPIO_20, GPIO_OUTPUT},//PIN_SENSOR_PH
	{GPIO_21, GPIO_OUTPUT},//PIN_BOMBA_AGUA
	{GPIO_22, GPIO_OUTPUT},//PIN_BOMBA_BASICA
	{GPIO_22, GPIO_OUTPUT},//PIN_BOMBA_ACIDA

	};
/*==================[internal functions declaration]=========================*/
/** 
 * @def controlar_humedad
 * @brief utilizamos esta funsion para controlar el esatdo de humedad de la planta, si le fata humedad el sensor pasa a 
 * alto el gpio y me enciende los motores luego vuelve a estar en bajo
*/
void controlar_humedad(void *pvParameters) {

    while (1) {
        int humedad = (gpio[0].dir);
        if (humedad == 1) { // Asumiendo que HIGH (1 lógico) indica baja humedad
            (gpio[2].pin, 1); // Encender la bomba de agua
			printf("se enciende la bomba de agua\n");
        } else {
            (gpio[2].pin, 0); // Apagar la bomba de agua
        }   
		 }
	}

/** 
 * @fn escribirValorEnPC() 
 * @brief Permite la salida por monitor serie de los valores analógicos leídos
*/
void escribirValorEnPc()
{
	uint16_t valorAnalogico = 0;
   // UartSendString(UART_PC, "valor ");
    UartSendString(UART_PC, (char *)UartItoa(valorAnalogico, 10));
    UartSendString(UART_PC, "\r");
}
/**
 * @def Control_PH
 * @brief utilizamos para verificar el pf de las plantas
 * */

void Control_PH(){
		uint16_t valorAnalogico = 0;

    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        AnalogInputReadSingle(CH1, &valorAnalogico);
		uint16_t ph= (14/valorAnalogico)/3;

        escribirValorEnPc();

		if (ph < phMin) {
            (gpio[3].pin, 1); // Encender la bomba de solución básica
			printf("se enciende la bomba de solucion basica\n");
            (gpio[4].pin, 0); // Asegurar que la bomba de solución ácida esté apagada
        } else if (ph > phMax) {
            (gpio[4].pin, 1); // Encender la bomba de solución ácida
			printf("Encender la bomba de solución ácida\n");
            (gpio[3].pin, 0); // Asegurar que la bomba de solución básica esté apagada
        } else {
            (gpio[3].pin, 0); // Apagar la bomba de solución básica
            (gpio[4].pin, 0); // Apagar la bomba de solución ácida
			printf("se apagan ambas bombas\n");
        }
    }

}

/**
 * @fn TeclaOn()
 * @brief Cambia el estado de la bandera booleana On
*/
void TeclaOn()
{
    on = !on;
}
/**
 * @fn TeclaHold()
 * @brief Cambia el estado de la bandera booleana Hold
*/
void TeclaHold()
{
    on = !on;
}
/**
 * @fn TeclaOnHold()
 * @brief Cambia el estado de las banderas booleanas On y Hold en función de la entrada por conexión de puerto
 * serie
*/

void FuncTimer_Humedad(void *param)
{
    vTaskNotifyGiveFromISR(HUMEDAD, pdFALSE); /* Envía una notificación de la tarea a medir*/
}

void FuncTimer_ph(void *param)
{
    vTaskNotifyGiveFromISR(P_h, pdFALSE); /* Envía una notificación de la tarea a medir*/
}

/*==================[external functions definition]==========================*/
void app_main(void){

/* timers para medir */
    timer_config_t timer_humedad = {
        .timer = TIMER_A,
        .period = REFRESCO_Humedad,
        .func_p = FuncTimer_Humedad, 
        .param_p = NULL
    };  TimerInit(&timer_humedad);

    /* timer para mostrar */
    timer_config_t timerph = {
        .timer = TIMER_B,
        .period = REFRESCO_Ph,
        .func_p = FuncTimer_ph,
        .param_p = NULL
    }; TimerInit(&timerph)

/*En este caso, a partir de crear la interrupcion iniciada por el SWITCH_1 elijo que la funcion estadoTeclas
     se va a ejecutar*/
    SwitchActivInt(SWITCH_1, TeclaOn, NULL);
    // SwitchActivInt('o', TeclaO, NULL);

    /*En este caso, a partir de crear la interrupcion iniciada por el SWITCH_2 elijo que la funcion estadoTeclas
    se va a ejecutar*/
    SwitchActivInt(SWITCH_2, TeclaHold, NULL);
    // SwitchActivInt('H', TeclaH, NULL);

xTaskCreate(&controlar_humedad, "controla la humedad", 512, NULL, 5, &ccontroldehumedad);

xTaskCreate(&Control_PH, "controla el ph", 1024, NULL, 4, &controldeph);


TimerStart(timer_humedad.timer);
TimerStart(timerph.timer);
}
/*==================[end of file]============================================*/
