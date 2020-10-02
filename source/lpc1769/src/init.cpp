#include "vcom.h"

//#include "IPS-100.h"
#include "codes.h"
#include "pinFunctions.h"
#include "macros.h"
#include "timers.h"
#include "I2C.h"
#include "GPIO.h"
#include "clock.h"
#include "UART.h"
#include "interrupt.h"
#include "DMA.h"


void VCOM::init_ST() {
	SB(STCTRL,2);
	ST_TIMEOUT(10);
}

void VCOM::init_I2C() {
	SB(PCONP, 7);
	I2B(PCLKSEL0,14,1);				//24MHz

	I2B(PINSEL1, 22, 1);			//SDA0
	I2B(PINSEL1, 24, 1);			//SCL0
	I2B(PINMODE1, 22, 2);			//Float
	I2B(PINMODE1, 24, 2);			//Float
	I2CPADCFG = 0;					//Standard drive mode

	I2C0SCLL = 480;					//100KHz 50%
	I2C0SCLH = 480;					//100KHz 50%

	I2C0CONCLR = 0x7F;				//Clear CTRL register
	I2C0CONSET = 0x40;				//I2C Enable
}

void VCOM::init_UART1() {
	SB(PCONP, 4);				//Enable PCLK
	U1TER = 0;					//Disable TX while configuration

	I2B(PCLKSEL0,18,0);			//24MHz
	U1LCR = 0x83;				//DLAB=1 to set baud rate
	U1DLM = 0;					//24MHz/115200/16
	U1DLL = 0x0D;
	U1FDR = 0x10;
	U1LCR = 0x03;				//DLAB = 0
	U1MCR = 0x00;

	I2B(PINSEL4,0,2);			//TXD1 P2.0 OUT
	I2B(PINSEL4,2,2);			//RXD1 P2.1	IN
	I2B(PINSEL4,4,2);			//CTS1 P2.2	IN
	I2B(PINSEL4,6,2);			//DCD1 P2.3 IN
	I2B(PINSEL4,8,2);			//DSR1 P2.4 IN
	I2B(PINSEL4,10,2);			//DTR1 P2.5 OUT
	I2B(PINSEL4,12,2);			//RI1  P2.6	IN
	I2B(PINSEL4,14,2);			//RTS1 P2.7	OUT

	I2B(PINMODE4,0,2);			//TXD1 Float
	I2B(PINMODE4,2,2);			//RXD1 Float
	I2B(PINMODE4,4,2);			//CTS1 Float
	I2B(PINMODE4,6,2);			//DCD1 Float
	I2B(PINMODE4,8,2);			//DSR1 Float
	I2B(PINMODE4,10,2);			//DTR1 Float
	I2B(PINMODE4,12,2);			//RI1  Float
	I2B(PINMODE4,14,2);			//DCD1 Float

	U1FCR = 0xC7;
	SB(U1TER, 7);
	while (VB(U1LSR, 0)) { U1RBR; }

	SB(U1IER, 0);				//Enable RX interrupt
	SB(ISER0, 6);				//Enable UART3 interrupt
}

void VCOM::init_UART3() {
	SB(PCONP, 25);				//Enable PCLK
	U3TER = 0;					//Disable TX while configuration

	I2B(PCLKSEL1,18,0);			//24MHz
	U3LCR = 0x83;				//DLAB=1 to set baud rate
	U3DLM = 0;					//24MHz/115200/16
	U3DLL = 0x0D;
	U3FDR = 0x10;
	U3LCR = 0x03;				//DLAB = 0

	I2B(PINSEL0,0,2);			//TXD3 P0.0
	I2B(PINSEL0,2,2);			//RXD3 P0.1
	I2B(PINMODE0,0,2);			//TXD3 Float
	I2B(PINMODE0,2,2);			//RXD3 Float

	U3FCR = 0x87;				//TODO put 0xC7
	SB(U3TER, 7);
	while (VB(U3LSR, 0)) { U3RBR; }

	SB(U3IER, 0);				//Enable RX interrupt
	SB(ISER0,8);				//Enable UART3 interrupt
}

void VCOM::init_MOTOROLA() {
	PINSEL0 &= 0xFFF0000F;		//GPIO ADD
	PINSEL1 &= 0xFFC00000;		//GPIO DATA / STR / RW / ACK

	PINMODE0 |= 0x000FFFF0;		//PULL-DOWN ADD
	PINMODE1 |= 0x0000FFFF;		//PULL-DOWN DATA
	PINMODE1 &= 0xFFC0FFFF;		//PULL-UP STR / RW / ACK

	FIO0DIR |= 0x03FF03FC;		//OUTPUT ADD / DATA

	FIO0PIN |= 0x03000000;

	I2B(PINSEL4, 20, 1);		//INTERRUPT sur EINT0
	I2B(PINMODE4, 20, 3);		//PULL-UP

	//ADD	pull-down	output		GPIO 0.2 - 0.9
	//DATA	pull-down	I/0			GPIO 0.16 - 0.23

	//STR 	pull-up		input		GPIO 0.24
	//RW 	pull-up 	input		GPIO 0.25
	//ACK	pull-up?	output		GPIO 0.26
}

