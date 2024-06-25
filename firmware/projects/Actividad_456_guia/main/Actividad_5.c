/*! @mainpage Actividad 4, 5 y 6 
 *
 * @section genDesc General Description
 *
 * visualizacion de digitos en pantalla led
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
 * |20/03/2024  | creacion de funsiones 
 * | 5/04/2024  | Document creation
 * 		                         
 *
 * @author Schvemer Eric (schvemereric@ingenieria.uner.edu.ar)
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
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/
/**
 * @brief Estructura del puerto GPIO, con un número de pin y una dirección
 */
typedef struct
{
	gpio_t pin;  /*!< GPIO pin number */
	io_t dir;	// GPIO direction '0' IN;  '1' OUT*/
/**
    @brief Creo vector de estructuras del tipo gpioConf_t que contienen los puertos GPIO_20, GPIO_21, GPIO_22 
     y GPIO_23. con sus correspondientes direcciones(entrada/salida)
    */
} gpioConf_t;

gpioConf_t gpioArray[4] =
		{
			{GPIO_20, GPIO_OUTPUT},
			{GPIO_21, GPIO_OUTPUT}, 
			{GPIO_22, GPIO_OUTPUT},
			{GPIO_23, GPIO_OUTPUT}}; 
/*==================[internal functions declaration]=========================*/

/**
 * @brief Establece el estado de los pines GPIO según un valor en código binario decimal (BCD), cambia de estado.
 * Esta función toma un valor BCD (que representa un número binario de 4 bits ) y configura los pines GPIO
 * en el arreglo según los bits individuales del valor BCD.
 *
 * @param bcd El valor BCD a interpretar, 8 bits.
 * @param gpioArray Un arreglo de estructuras `gpioConf_t` que representa los pines GPIO.
 */
void setGpioState(uint8_t bcd, gpioConf_t *gpioArray) //cambia de estado 
{
	for (int i = 0; i < 4; i++)
	{/*Utilizo una máscara, corriendo 1 i veces y comparando en el for cada dígito y dependiendo
        el resultado de la operación and seteo el estado de los puertosGPIO correspondiente*/
		if ((bcd >> i) & 1)
		{
			GPIOOn(gpioArray[i].pin); //deveria considerar alto y bajo? 
		}
		else
			GPIOOff(gpioArray[i].pin);
	}
}

