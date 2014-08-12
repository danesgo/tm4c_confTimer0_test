/*
 * main.c
 *
 * Autor: Rodrigo Chang
 * Fecha: 11 de agosto de 2014
 *
 * Programa que configura el TIMER0 en modo periodico y hace toggle a un LED en el Launchpad
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/sysctl.h"

#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"

// Prototipos de funciones
void ConfigurarTimer(uint32_t load);
void ConfigurarLEDs(void);

// Definiciones
#define PF3	HWREG(GPIO_PORTF_BASE + 32)

int main(void) {
	// Configurar el reloj a 40MHz
	SysCtlClockSet(SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ | SYSCTL_USE_PLL | SYSCTL_SYSDIV_5);

	// Configurar LED para hacer toggle
	ConfigurarLEDs();
	// Configurar el TIMER0 para 1s@40MHz
	ConfigurarTimer(40000000);
	// Habilitar las interrupciones globales
	IntMasterEnable();

	// Ciclo principal
	while (1) {
		// Esperar las interrupciones
	}
}

/*
 * Configura el LED para hacer toggle
 */
void ConfigurarLEDs(void) {
	// Habilitar reloj al periferico
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	// Configurar como salida
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);
	// Apagar el LED
	//GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 0x00);
	PF3 = 0;
}

/*
 * Configura el timer periodico con un perido variable
 */
void ConfigurarTimer(uint32_t load) {
	// Activar el reloj del periferico
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	// Configurar el timer para 32 bits en modo periodico
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	// Cargar el valor del timeout
	TimerLoadSet(TIMER0_BASE, TIMER_A, load - 1);
	// Activar la interrupcion de timeout
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntEnable(INT_TIMER0A);
	IntPrioritySet(INT_TIMER0A, 5);
	// Iniciar el timer
	TimerEnable(TIMER0_BASE, TIMER_A);
}

/*
 * Rutina de interrupcion de TIMER0 periodico
 */
void Int_Timer0_Handler(void) {
	// Borrar la interrupcion
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	// Hacer toggle al LED
	PF3 ^= 0xff;
}
