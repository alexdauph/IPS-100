#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "vcom.h"
#include "codes.h"
#include "macros.h"
#include "timers.h"
#include "I2C.h"
#include "interrupt.h"
#include "UART.h"
#include "GPIO.h"
#include "DMA.h"
using namespace std;

VCOM vLink;																//VCOM class instance. Handles data headers (see "codes.h")

Operation_t operations[11] = {
		{ LPC_RQST, &VCOM::init },
		{ LPC_PING, &VCOM::ping },
		{ I2C_RD, &VCOM::i2c_read },
		{ I2C_WR, &VCOM::i2c_write },
		{ UART_RD, &VCOM::uart_read },
		{ UART_WR, &VCOM::uart_write },
		{ MOTOROLA_RD, &VCOM::moto_read },
		{ MOTOROLA_WR, &VCOM::moto_write },
		{ UART_MOD_PARAM, &VCOM::uart_mod_param },
		{ UART_MOD_LINES, &VCOM::uart_mod_lines },
		{ UART_GET_CNT, &VCOM::uart_get_count },

};

void WFD(void) {														//Wait For Data
	uint8_t request;

	if (vcom_bread(&request, 1)) {
		for (uint8_t i = 0; i < ACTION_COUNT; i++) {
			if (request == operations[i].rqst) {
				(vLink.*(operations[i].action))();
				return;
			}
		}
	}
}

VCOM::VCOM() {

}

void VCOM::init(void) {
	ST_init();
	i2c_init();
	uart1_init();
	uart3_init();
	motorola_init();

	uart1.registers = (UartReg_t*)0x40010000;
	uart1.ID = RS232;
	uart1.index = 0;
	uart1.count = 0;
	uart1.SavedLSR = 0;
	uart1.newData = false;
	uart1.registers->RBR;
	SB(uart1.registers->FCR, 1);
	uart1.registers->IIR;

	uart3.registers = (UartReg_t*)0x4009C000;
	uart3.ID = RS232;
	uart3.index = 0;
	uart3.count = 0;
	uart3.SavedLSR = 0;
	uart3.newData = false;
	uart3.registers->RBR;
	SB(uart3.registers->FCR, 1);
	uart3.registers->IIR;

	com_send_byte(LPC_READY);
}

void VCOM::ping(void) {
	com_send_byte(LPC_READY);
}

//**************************************************************************************

uint8_t VCOM::com_get_byte(void) {
	uint8_t buffer[1];
	com_get_data(buffer, 1);
	return buffer[0];
}

bool VCOM::com_send_byte(uint8_t byte) {
	uint8_t buffer[1] = { byte };
	return com_send_data(buffer, 1) == 1;
}

uint8_t VCOM::com_get_data(uint8_t* buffer, uint8_t count) {
	return vcom_bread(buffer, count);
}

uint8_t VCOM::com_send_data(uint8_t* buffer, uint8_t count) {
	return vcom_write(buffer, count);
}

//**************************************************************************************

bool VCOM::wait_vcom(uint32_t microSeconds) {
	uint8_t result = false;
	uint32_t timeOut = STRELOAD;

	ST_TIMEOUT(microSeconds);
	SB(STCTRL, 0);

	while (!VB(STCTRL, 16)) {
		if (GetCount()) {
			result = true;
			break;
		}
	}

	CB(STCTRL, 0);
	STCURR = 0;
	STRELOAD = timeOut;

	return result;
}

bool VCOM::wait_uart_rx(Uart_e uartID, uint32_t microSeconds) {
	uint8_t result = false;
	uint32_t timeOut = STRELOAD;
	Uart_t* uart;

	if (uartID == RS232) { uart = &uart1; }
	else if (uartID == UART3) { uart = &uart3; }

	ST_TIMEOUT(microSeconds);
	SB(STCTRL, 0);

	while (!VB(STCTRL, 16)) {
		if (uart->newData == true) {
			result = true;
			break;
		}
	}

	CB(STCTRL, 0);
	STCURR = 0;
	STRELOAD = timeOut;

	return result;
}

bool VCOM::wait_uart_tx(Uart_e uartID, uint32_t microSeconds) {
	uint8_t result = false;
	uint32_t timeOut = STRELOAD;
	Uart_t* uart;

	if (uartID == RS232) { uart = &uart1; }
	else if (uartID == UART3) { uart = &uart3; }

	ST_TIMEOUT(microSeconds);
	SB(STCTRL, 0);

	while (!VB(STCTRL, 16)) {
		if (VB(uart->registers->LSR, 5) == true) {
			result = true;
			break;
		}
	}

	CB(STCTRL, 0);
	STCURR = 0;
	STRELOAD = timeOut;

	return result;
}

bool VCOM::wait_i2c(uint8_t val, uint32_t microSeconds) {
	uint8_t result = false;
	uint32_t timeOut = STRELOAD;

	ST_TIMEOUT(microSeconds);
	SB(STCTRL, 0);

	while (!VB(STCTRL, 16)) {
		if (I2C0STAT == val) {
			result = true;
			break;
		}
	}

	CB(STCTRL, 0);
	STCURR = 0;
	STRELOAD = timeOut;

	return result;
}


bool VCOM::wait_moto(uint32_t tensNanoSeconds) {
	uint8_t result = false;
	uint32_t timeOut = STRELOAD;

	ST_TIMEOUT_10E8(tensNanoSeconds);
	SB(STCTRL, 0);

	while (!VB(STCTRL, 16)) {
		if (!VB(FIO0PIN, 26) && result == 0) {
			result = true;
			//break;
		}
	}

	CB(STCTRL, 0);
	STCURR = 0;
	STRELOAD = timeOut;

	return result;
}

//**************************************************************************************

void VCOM::i2c_read(void) {
	uint8_t address;
	uint8_t dataCount;
	uint8_t header[2] = {};
	uint8_t dataBuffer[256] = {};
	uint8_t result = I2C_OK;

	com_send_byte(I2C_READY);

	if (wait_vcom(20000)) {
		if (com_get_data(header, 2) == 2) {
			address = header[0];
			dataCount = header[1];
		}

		result = i2c_get(address, dataBuffer, dataCount);
		com_send_byte(result);

		for (uint16_t i = 0; i < dataCount; i += 32) {
			if (wait_vcom(20000)) {										//Attendre l'ordre de transmission de l'application
				if (com_get_byte() != 0x01) { return; }					//0x01 = OK
				com_send_data(dataBuffer + i, min(dataCount - i, 32));	//Envoyer 32 octets sur le lien vcom
			}
			else { break; }												//L'application ne veux pas recevoir les octets
		}
	}
}

void VCOM::i2c_write(void) {
	uint8_t address;
	uint8_t dataCount;
	uint8_t header[2] = {};
	uint8_t dataBuffer[256] = {};
	uint8_t result = I2C_OK;

	com_send_byte(I2C_READY);

	if (wait_vcom(20000)) {
		if (com_get_data(header, 2) == 2) {
			address = header[0];
			dataCount = header[1];
		}

		for (uint16_t i = 0; i < dataCount; i += 32) {
			com_send_byte(0x01);
			if (wait_vcom(20000)) {
				if (com_get_data(&dataBuffer[i], min(dataCount - i, 32)) != min(dataCount - i, 32)) { return; }
			}
			else { return; }
		}

		result = i2c_send(address, dataBuffer, dataCount);
		com_send_byte(result);
	}
}

void VCOM::uart_read(void) {
	uint8_t header[4] = {};
	uint16_t dataCount;
	uint8_t result = UART_OK;
	Uart_t* uart;

	//header[0] = UART select
	//header[1] = Count LSB
	//header[2] = Count MSB
	//header[3] = Wait for data

	com_send_byte(UART_READY);

	if (wait_vcom(20000)) {
		if (com_get_data(header, 4) == 4) {

			if (header[0] == RS232) { uart = &uart1; }
			else if (header[0] == UART3) { uart = &uart3; }
			dataCount = (header[1] << 0) | (header[2] << 8);

			if (uart->count == 0 && header[3] == true) {				//Attente du premier octet (RecevoirUnVecteur)
				while (!uart->newData) {								//Vérifier si il y a une donnée UART disponible
					if (GetCount()) {									//Donnée sur le lien VCOM
						if (com_get_byte() == 0x55) {					//Ordre d'avorter l'attente du premier octet
							SB(uart->registers->IER, 0);				//Réactiver les interruptions
							return;
						}
					}
				}
			}

			while (1) {
				uart->newData = false;

				if ((uart->SavedLSR & 0x0E)) {								//Verifier les erreurs de transmission
					if (VB(uart->SavedLSR, 1)) { result = UART_OVERRUN; }	//Erreur d'octets écrasés
					if (VB(uart->SavedLSR, 2)) { result = UART_PARITY; }	//Erreur de parité
					if (VB(uart->SavedLSR, 3)) { result = UART_FRAMING; }	//Erreur de cadrage
					uart->registers->RBR;								//Vider le registre RBR
					SB(uart->registers->FCR, 1);						//Vider le buffer de réception
					uart->count = 0;									//Effacer les octets en attente
					uart->SavedLSR = 0x00;								//Effacer les erreurs de transmission
					dataCount = 0;
					break;
				}

				if (!wait_uart_rx((Uart_e)header[0], 100000)) {			//Aucune réception de données pour 100ms
					if (uart->count >= dataCount) { break; }
					else if (uart->count > 0) { result = UART_UFLOW; }	//Moins de données que prévu
					else { result = UART_EMPTY; }						//Aucune donnée disponible
					dataCount = uart->count;
					break;
				}
			}

			CB(uart->registers->IER, 0);								//Désactiver les interruptions

			header[0] = result;
			header[1] = (dataCount & 0x00FF) >> 0;
			header[2] = (dataCount & 0xFF00) >> 8;
			com_send_data(header, 3);									//Envoyer le resultat à l'application

			for (uint16_t i = 0; i < dataCount; i += 32) {
				uint8_t toSend = min(dataCount - i, 32);
				uint8_t tempBuffer[32];

				if (wait_vcom(20000)) {
					if (com_get_byte() != 0x01) { return; }

					if(uart->index + toSend > 2048) {
						for(uint16_t j=0; j<toSend; j++) {
							tempBuffer[j] = uart->rxData[((uart->index + j) % 2048)];
						}
						com_send_data(tempBuffer, toSend);				//Pour gerer "les wraps around" dans le FIFO software
					}
					else {
						com_send_data(uart->rxData + uart->index, toSend);	//Envoyer 32 octets sur le lien vcom
					}

					uart->count -= toSend;
					uart->index += toSend;
					if(uart->index>2047) { uart->index %= 2048; }

				}
				else { break; }											//L'application ne veux pas recevoir les octets
			}

			if(uart->count == 0) {uart->index = 0;}						//Remettre le pointeur FIFO à 0
			SB(uart->registers->IER, 0);								//Réactiver les interruptions
		}
	}
}

void VCOM::uart_write(void) {
	uint8_t header[3] = { };
	uint16_t dataCount;
	uint8_t result = UART_OK;
	Uart_t* uart;

	com_send_byte(UART_READY);

	if (wait_vcom(20000)) {
		if (com_get_data(header, 3) == 3) {								//Sélection du UART + nombre d'octets
			if (header[0] == RS232) { uart = &uart1; }
			else if (header[0] == UART3) { uart = &uart3; }
			dataCount = (header[1] << 0) | (header[2] << 8);
		}

		for (uint16_t i = 0; i < dataCount; i += 32) {					//Réception des octets
			com_send_byte(0x01);
			if (wait_vcom(20000)) {
				if (com_get_data(&uart->txData[i], min(dataCount - i, 32)) != min(dataCount - i, 32)) { return; }	//Lire 32 octets sur le lien VCOM
			}
			else { return; }
		}

		for (uint16_t i = 0; i < dataCount; i++) {						//Envoi des octets
			if (header[0] == RS232 && VB(U1MCR, 7)) {
				while (!VB(U1LSR, 5)) {									//TODO twice same while loop
					if (GetCount()) {									//Donnée sur le lien vcom
						if (com_get_byte() == 0x55) {					//Ordre d'avorter l'envoi
							result = UART_ABORT;
							break;
						}
					}
				}
			}
			if (!wait_uart_tx((Uart_e)header[0], 100000)) {	break; }	//Attendre TX libre
			uart->registers->THR = uart->txData[i];						//Envoyer l'octet
		}

		com_send_data(&result, 1);
	}
}

void VCOM::moto_read(void) {
	uint8_t header[2] = {};
	uint8_t result = MOTOROLA_OK;

	//header[0] = address

	com_send_byte(MOTOROLA_READY);

	if (wait_vcom(20000)) {
		if (com_get_data(header, 1) == 1) {

			FIO0DIR &= 0xFF00FFFF;										//DATA input;
			FIO0PIN = ((FIO0PIN & 0xFFFFFC03) | (header[0] << 2));		//ADD on GPIO0
			FIO0SET = 1 << 25;											//RW = Read
			FIO0CLR = 1 << 24;											//STR actif

			if (wait_moto(40)) {
				header[0] = (FIO0PIN >> 16) & 0xFFFF;					//Capturer l'octet
			}
			else {
				result = MOTOROLA_FAIL;									//Aucun périphérique n'a répondu
			}

			FIO0SET = 1 << 24;											//STR inactif
			FIO0SET = 1 << 25;											//RW inactif

			header[1] = result;
			com_send_data(header, 2);									//Envoyer octet et resultat
		}
	}
}

void VCOM::moto_write(void) {
	uint8_t header[2] = {};
	uint8_t result = MOTOROLA_OK;

	//header[0] = address
	//header[1] = data

	com_send_byte(MOTOROLA_READY);

	if (wait_vcom(20000)) {
		if (com_get_data(header, 2) == 2) {

			FIO0DIR |= 0x00FF0000;										//DATA output;
			FIO0PIN = ((FIO0PIN & 0xFF00FC03) | (header[0] << 2) | (header[1] << 16));	//ADD + DATA on GPIO0
			FIO0CLR = 1 << 25;											//RW = Write
			FIO0CLR = 1 << 24;											//STR actif

			if (wait_moto(40)) {
				result = MOTOROLA_OK;									//Écriture ok
			}
			else {
				result = MOTOROLA_FAIL;									//Aucun périphérique n'a répondu
			}

			FIO0SET = 1 << 24;											//STR inactif
			FIO0SET = 1 << 25;											//RW inactif

			header[0] = header[1];
			header[1] = result;
			com_send_data(header, 2);									//Envoyer resultat
		}
	}
}

void VCOM::uart_mod_param(void) {
	uint8_t header[3] = { };
	uint8_t result = UART_OK;
	Uart_t* uart;

	com_send_byte(UART_READY);

	if (wait_vcom(20000)) {
		if (com_get_data(header, 3) == 3) {
			if (header[0] == RS232) { uart = &uart1; }
			else if (header[0] == UART3) { uart = &uart3; }

			uart->registers->LCR = (uart->registers->LCR & 0xC0) | header[1];											//Bit count, stop bits, parity

			SB(uart->registers->LCR, 7);																				//Baud Rate
			switch (header[2]) {
			case 0: { uart->registers->DLM = 0x13; uart->registers->DLL = 0x88; uart->registers->FDR = 0x10; break; }	//300
			case 1: { uart->registers->DLM = 0x04; uart->registers->DLL = 0xE2; uart->registers->FDR = 0x10; break; }	//1200
			case 2: { uart->registers->DLM = 0x02; uart->registers->DLL = 0x71; uart->registers->FDR = 0x10; break; }	//2400
			case 3: { uart->registers->DLM = 0x00; uart->registers->DLL = 0xD0; uart->registers->FDR = 0x21; break; }	//4800
			case 4: { uart->registers->DLM = 0x00; uart->registers->DLL = 0x68; uart->registers->FDR = 0x21; break; } 	//9600
			case 5: { uart->registers->DLM = 0x00; uart->registers->DLL = 0x34; uart->registers->FDR = 0x21; break; } 	//19200
			case 6: { uart->registers->DLM = 0x00; uart->registers->DLL = 0x1A; uart->registers->FDR = 0x21; break; } 	//34800
			case 7: { uart->registers->DLM = 0x00; uart->registers->DLL = 0x0D; uart->registers->FDR = 0x10; break; } 	//115200
			default: { result = UART_ERROR; }
			}
			CB(uart->registers->LCR, 7);

			com_send_data(&result, 1);
		}
	}
}

void VCOM::uart_mod_lines(void) {
	uint8_t header[1] = { };
	uint8_t result = UART_OK;
	uint8_t RS232Lines;
	uint8_t returnBuf[2] = {};

	com_send_byte(UART_READY);

	if (wait_vcom(20000)) {
		if (com_get_data(header, 1) == 1) {
			RS232Lines = header[0];

			IB(U1MCR, 0, VB(RS232Lines, 0));							//Ligne DTR
			IB(U1MCR, 1, VB(RS232Lines, 1));							//Ligne RTS
			IB(U1MCR, 6, VB(RS232Lines, 2));							//Flow RTS automatique
			IB(U1MCR, 7, VB(RS232Lines, 2));							//Flow CTS automatique
			RS232Lines = (U1MCR & 0x03) | (U1MSR & 0xF0);				//Lire les lignes d'état pour les envoyer à l'application

			returnBuf[0] = RS232Lines;
			returnBuf[1] = result;
			com_send_data(returnBuf, 2);
		}
	}
}

void VCOM::uart_get_count(void) {										//Nombre d'octet en attente de lecture
	uint8_t header[1] = { };
	uint8_t result = UART_OK;
	uint8_t returnBuf[3] = {};

	com_send_byte(UART_READY);

	if (wait_vcom(20000)) {
		if (com_get_data(header, 1) == 1) {

			if (header[0] == RS232) {									//UART1 ou UART3
				returnBuf[0] = (uart1.count & 0x00FF) >> 0;
				returnBuf[1] = (uart1.count & 0xFF00) >> 8;
			}
			else if (header[0] == UART3) {								//UART1 ou UART3
				returnBuf[0] = (uart3.count & 0x00FF) >> 0;
				returnBuf[1] = (uart3.count & 0xFF00) >> 8;
			}

			returnBuf[2] = result;
			com_send_data(returnBuf, 3);
		}
	}
}

uint8_t VCOM::i2c_get(uint8_t address, uint8_t* buffer, uint8_t count) {
	I2C0CONSET = 0x20;
	if (!wait_i2c(0x08, 20000)) {
		I2C0CONSET = 0x10;
		I2C0CONCLR = 0x28;
		return I2C_NO_START;
	}

	I2C0DAT = address | 0x01;											//Slave address + read
	I2C0CONCLR = 0x38;
	if (!wait_i2c(0x40, 20000)) {
		I2C0CONSET = 0x10;
		I2C0CONCLR = 0x28;
		return I2C_NO_SLAVE;
	}

	for (uint8_t i = 0; i < count - 1; i++) {
		I2C0CONSET = 0x04;
		I2C0CONCLR = 0x38;
		if (!wait_i2c(0x50, 20000)) {
			I2C0CONSET = 0x10;
			I2C0CONCLR = 0x28;
			return I2C_NACK;
		}
		buffer[i] = I2C0DAT;
	}

	I2C0CONCLR = 0x3C;
	if (!wait_i2c(0x58, 20000)) {
		I2C0CONSET = 0x10;
		I2C0CONCLR = 0x28;
		return I2C_NACK;
	}
	buffer[count - 1] = I2C0DAT;

	I2C0CONSET = 0x10;
	I2C0CONCLR = 0x28;
	return I2C_OK;
}

uint8_t VCOM::i2c_send(uint8_t address, uint8_t* buffer, uint8_t count) {
	I2C0CONSET = 0x20;
	if (!wait_i2c(0x08, 20000)) {
		I2C0CONSET = 0x10;
		I2C0CONCLR = 0x28;
		return I2C_NO_START;
	}

	I2C0DAT = address | 0x00;											//Slave address + write
	I2C0CONCLR = 0x38;
	if (!wait_i2c(0x18, 20000)) {
		I2C0CONSET = 0x10;
		I2C0CONCLR = 0x28;
		return I2C_NO_SLAVE;
	}

	for (uint8_t i = 0; i < count; i++) {
		I2C0DAT = buffer[i];
		I2C0CONCLR = 0x38;
		if (!wait_i2c(0x28, 20000)) {
			I2C0CONSET = 0x10;
			I2C0CONCLR = 0x28;
			return I2C_NACK;
		}
	}

	I2C0CONSET = 0x10;
	I2C0CONCLR = 0x28;
	return I2C_OK;
}



void UART1_IRQHandler(void) {
	vLink.uart1.newData = true;

	while (1) {
		vLink.uart1.SavedLSR = (vLink.uart1.SavedLSR & 0x0E) | U1LSR;

		if (!VB(vLink.uart1.SavedLSR, 0)) {
			U1IIR;
			return;
		}

		if (vLink.uart1.count < 2048) {
			vLink.uart1.rxData[vLink.uart1.count++] = U1RBR;
			vLink.uart1.SavedLSR |= (U1LSR & 0x0E);
		}
		else {															//Débordement du buffer
			vLink.uart1.SavedLSR |= 0x02;								//Overrun error
			U1RBR;														//Vider le registre de réception
			SB(U1FCR, 1);												//Vider le buffer de réception
			U1IIR;
			return;
		}
	}
}

void UART3_IRQHandler(void) {
	vLink.uart3.newData = true;

	while (1) {
		vLink.uart3.SavedLSR = (vLink.uart3.SavedLSR & 0x0E) | U3LSR;

		if (!VB(vLink.uart3.SavedLSR, 0)) {
			U3IIR;
			return;
		}

		if (vLink.uart3.count < 2048) {
			vLink.uart3.rxData[(vLink.uart3.index + vLink.uart3.count++) % 2048] = U3RBR;
			vLink.uart3.SavedLSR |= (U3LSR & 0x0E);						//Les erreurs sont parfois captées après l'octet
		}
		else {															//Débordement du buffer
			vLink.uart3.SavedLSR |=  0x02;								//Overrun error
			U3RBR;														//Vider le registre de réception
			SB(U3FCR, 1);												//Vider le buffer de réception
			U3IIR;
			return;
		}
	}
}
