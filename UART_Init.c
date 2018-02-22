/*
 * UART_Init.c
 *
 *  Created on: Feb 20, 2018
 *      Author: joe
 */

#include "driverlib.h"
#include "UART_Init.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- UART Configuration Parameter ---
const eUSCI_UART_Config uartConfig =
{
        //Configured for 9600 Baud Rate
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        1284,                                     // BRDIV = ???
        0,                                       // UCxBRF = ?
        0,                                       // UCxBRS = ?
        EUSCI_A_UART_NO_PARITY,                  // No Parity
        EUSCI_A_UART_LSB_FIRST,                  // MSB First
        EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
        EUSCI_A_UART_MODE,                       // UART mode
        EUSCI_A_UART_LOW_FREQUENCY_BAUDRATE_GENERATION  // Low Frequency Mode
};

int fputc(int _c, register FILE *_fp)
{
    while(!(UCA0IFG&UCTXIFG));
    UCA0TXBUF = (unsigned char) _c;
    return((unsigned char)_c);
}
int fputs(const char *_ptr, register FILE *_fp)
{
    unsigned int i, len;
    len = strlen(_ptr);
    for(i=0 ; i<len ; i++)
    {
        while(!(UCA0IFG&UCTXIFG));
        UCA0TXBUF = (unsigned char) _ptr[i];
    }
return len;
}

void UART_init(void){

    // Selecting P1.2 and P1.3 in UART mode.
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);
    MAP_UART_initModule(EUSCI_A0_BASE, &uartConfig);         // Configuring UART Module
    MAP_UART_enableModule(EUSCI_A0_BASE);               // Enable UART module
}

