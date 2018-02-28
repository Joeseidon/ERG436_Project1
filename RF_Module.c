/*
 * RF_Module.c
 *
 *  Created on: Feb 28, 2018
 *      Author: joe
 */

#include "driverlib.h"
#include "msprf24.h"
#include "RF_Module.h"

uint8_t addr[5];

void RF_IRQ_Init(void){
    //setup IRQ
    P5DIR &= ~BIT0;  // IRQ line is input
    P5OUT |= BIT0;   // Pull-up resistor enabled
    P5REN |= BIT0;
    P5IES |= BIT0;   // Trigger on falling-edge
    P5IFG &= ~BIT0;  // Clear any outstanding IRQ
    P5IE |= BIT0;    // Enable IRQ interrupt
    P5IFG &= 0; // clear flag

    MAP_Interrupt_enableInterrupt(INT_PORT5);
}

void RF_Module_Init_RX(void){
    /* Initial values for nRF24L01+ library config variables */
   rf_crc = RF24_EN_CRC | RF24_CRCO; // CRC enabled, 16-bit
   rf_addr_width      = 5;
   rf_speed_power     = RF24_SPEED_1MBPS | RF24_POWER_0DBM;
   rf_channel         = 105;

   msprf24_init();
   msprf24_set_pipe_packetsize(0, 32);
   msprf24_open_pipe(0, 1);  // Open pipe#0 with Enhanced ShockBurst

   // Set our RX address
   addr[0] = 0xEE; addr[1] = 0xAD; addr[2] = 0xBE; addr[3] = 0xEF; addr[4] = 0x00;
   w_rx_addr(0, addr);

   // Receive mode
   if (!(RF24_QUEUE_RXEMPTY & msprf24_queue_state())) {
       flush_rx();
   }

   msprf24_activate_rx();
}

