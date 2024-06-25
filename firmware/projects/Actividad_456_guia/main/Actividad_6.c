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

} gpioConf_t;

/**
*  @brief Creo vector de estructuras del tipo gpioConf_t que contienen los puertos GPIO_20, GPIO_21, GPIO_22 
*  y GPIO_23. con sus correspondientes direcciones(entrada/salida)
    */
	gpioConf_t gpioArray[4] =
		{
			{GPIO_20, GPIO_OUTPUT},
			{GPIO_21, GPIO_OUTPUT}, 
			{GPIO_22, GPIO_OUTPUT},
			{GPIO_23, GPIO_OUTPUT}}; 

 /**
    @brief Creo el vector de estructuras del tipo gpioConf_t que contienen los puertos GPIO_9, GPIO_18, GPIO_19 
     con sus correspondientes direcciones
    */
gpioConf_t gpioMap[3] =
		{
			{GPIO_19, GPIO_OUTPUT},
			{GPIO_18, GPIO_OUTPUT}, //va a la pantalla
			{GPIO_9, GPIO_OUTPUT},
		};


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

/**
 * @brief Muestra datos en un display de 7 segmentos.
 *
 * Esta función toma un número entero sin signo de 32 bits (`data`), la cantidad de dígitos a mostrar (`digits`),
 * un arreglo de configuración de pines GPIO (`gpioArray`), otro arreglo de configuración de pines GPIO (`gpioMap`),
 * y un puntero a un arreglo de números en formato BCD (`bcd_number`). 
 * muestra en el lcd el dato infresado
 *
 * @param data     El número que se mostrará en el display.
 * @param digits   La cantidad de dígitos en el display.
 * @param gpioArray Arreglo de configuración de pines GPIO para controlar los segmentos del display.
 * @param gpioMap  Arreglo de configuración de pines GPIO para controlar los dígitos de los pines del display. 
 * @param bcd_number Puntero al arreglo de números en formato BCD.
 */
void mostrarDisplay ( uint32_t data, uint8_t digits, gpioConf_t *gpioArray,  gpioConf_t *gpioMap ,uint8_t *bcd_number )
{
	convertToBcdArray(data,digits,bcd_number);
	for (int i=0;i<digits;i++){
			setGpioState(bcd_number[i],gpioArray);
			GPIOOn(gpioMap[i].pin);
			GPIOOff(gpioMap[i].pin);
	}
	
}


/*==================[external functions definition]==========================*/
void app_main(void)
{
	uint32_t numero = 123;
	uint8_t digits = 3;
	uint8_t bcd_number[digits];

	convertToBcdArray(numero, digits, bcd_number);
     /*
     Creo vector de estructuras del tipo gpioConf_t que contienen los puertos GPIO_20, GPIO_21, GPIO_22 
     y GPIO_23. con sus correspondientes direcciones(entrada/salida)
    */
	gpioConf_t gpioArray[4] =
		{
			{GPIO_20, GPIO_OUTPUT},
			{GPIO_21, GPIO_OUTPUT}, // va a ala pantalla
			{GPIO_22, GPIO_OUTPUT},
			{GPIO_23, GPIO_OUTPUT}}; //  vector de 4 estructuras gpioConf_t

	//inicializo los gpio
	for (int i = 0; i < 4; i++)
	{
		GPIOInit(gpioArray[i].pin, gpioArray[i].dir);
	}
	

gpioConf_t gpioMap[3] =
		{
			{GPIO_19, GPIO_OUTPUT},
			{GPIO_18, GPIO_OUTPUT}, //  va a ala pantalla
			{GPIO_9, GPIO_OUTPUT},
		};

	//inicializo los gpio
	for(int i = 0; i < 3; i++)
	{
		GPIOInit(gpioMap[i].pin, gpioMap[i].dir);
	};
mostrarDisplay(numero, digits,gpioArray,gpioMap,bcd_number);


}


/*==================[end of file]============================================*/