/*! @mainpage Actividad 4, 5 y 6 
 *
 * @section genDesc General Description
 *
 * convertir el dato recibido a BCD, guardando cada uno de los dígitos
 *  de salida en el arreglo pasado como puntero.
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

/*==================[internal functions declaration]=========================*/

/**
 * @brief Esta función toma un número entero y lo convierte en un arreglo de dígitos BCD.
 *
 * @param data El número entero a convertir,sin signo de 32 bits.
 * @param digits La cantidad de dígitos en el número,sin signo de 8 bits.
 * @param bcd_number El arreglo donde se almacenará la representación BCD.de 8 bist.
 * @return 0 si la conversión fue exitosa.
 */
int8_t convertToBcdArray(uint32_t data, uint8_t digits, uint8_t *bcd_number)
{
	for (int i = digits - 1; i >= 0; --i)
	{
		bcd_number[i] = data % 10;
		data /= 10;
	}
	return 0;
}

