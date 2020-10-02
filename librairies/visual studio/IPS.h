#pragma once
#include <iostream>
#include <Windows.h>
using namespace std;


enum LogLevel_e {
	DEBUG,
	SUCCES,
	AVERTISSEMENT,
	ERREUR,
	ECHEC,
	INVALIDE,
};

struct LogMessage_t {
	uint8_t ID;
	LogLevel_e level;
	string name;
	string text;
};

class SerialPort;


class IPS
{
public:
	/* Liste des differents UART */
	enum Uart_e {
		RS232 = 1,
		UART3 = 3,
	};

private:
	friend class TB8;

	/* Variables privées du lien COM virtuel */
	SerialPort* mVirtualPort;
	uint16_t mPort;
	BOOL mConnecting = true;
	BOOL mInitialized = false;
	uint8_t mRs232Baud = 0x07;
	uint8_t mRs232LCR = 0x03;
	uint8_t mRs232Lines = 0x00;							// uint8_t = [7]=DCD [6]=RI [5]=DCE [4]=CTS [3]X [2]=FlowControl [1]=DTR [0]=RTS
	uint8_t mUart3Baud = 0x07;
	uint8_t mUart3LCR = 0x03;


public:
	/* Constructeurs et destructeur */
	IPS();
	IPS(uint8_t port);
	~IPS();

	/* Information par rapport au code de résultat */
	LogLevel_e GetCodeLevel(uint8_t code);
	string GetCodeLevelText(uint8_t code);
	string GetCodeText(uint8_t code);

	/* Transforme les parametres recus en trames génériques afin d'appeler les méthodes privées */
	uint8_t gpio_Read(uint8_t address, uint8_t* byte);
	uint8_t gpio_Write(uint8_t address, uint8_t byte);
	uint8_t memory_Read(uint16_t address, uint8_t* buffer, uint8_t count);
	uint8_t memory_Write(uint16_t address, uint8_t* buffer, uint8_t count);
	uint8_t uart3_Read(uint8_t* buffer, uint16_t count);
	uint8_t uart3_Read(uint8_t* buffer, uint16_t* count);
	uint8_t uart3_Write(uint8_t* buffer, uint16_t count);
	uint8_t rs232_Read(uint8_t* buffer, uint16_t count);
	uint8_t rs232_Read(uint8_t* buffer, uint16_t* count);
	uint8_t rs232_Write(uint8_t* buffer, uint16_t count);
	uint8_t motorola_Read(uint8_t address, uint8_t* byte);
	uint8_t motorola_Write(uint8_t address, uint8_t byte);
	uint8_t i2c_Read(uint8_t address, uint8_t* buffer, uint8_t count);
	uint8_t i2c_Write(uint8_t address, uint8_t* buffer, uint8_t count);

	/* Appel les méthodes privées de modification/lecture des paramètres UART et lecture du nombre d'octet en attente */
	uint8_t uart_SetParam(Uart_e uart);
	uint8_t uart_GetParam(Uart_e uart);
	uint8_t uart3_GetCount(uint16_t* count);
	uint8_t rs232_GetCount(uint16_t* count);

	/* Appel les méthodes privées de controles des lignes ou des paramètres RS232 */
	void rs232_EnableFlowControl(void);
	void rs232_DisableFlowControl(void);
	void rs232_EnableRTS(void);
	void rs232_EnableDTR(void);
	void rs232_DisableRTS(void);
	void rs232_DisableDTR(void);
	BOOL rs232_GetCTS(void);
	BOOL rs232_GetDCE(void);
	BOOL rs232_GetRI(void);
	BOOL rs232_GetDCD(void);

private:
	/* Overture d'un mPort COM */
	uint8_t com_Connect(void);
	uint8_t com_Connect(uint8_t port);

	/* Methodes de communication directe avec le LPC */
	BOOL com_Wait(uint8_t breakCondition);
	uint8_t com_GetByte(void);
	BOOL com_SendByte(uint8_t byte);
	uint8_t com_GetData(uint8_t* buffer, uint8_t count);
	uint8_t com_SendData(uint8_t* buffer, uint8_t count);
	uint8_t com_Request(uint8_t command, uint8_t reply, uint8_t* var);

	/* Détection et gestion d'erreurs critiques */
	uint8_t com_Verify(uint8_t result);
	void ErrorHandler(uint8_t error);

	/* Transforme les trames génériques en blocs de données */
	uint8_t uart_Write(Uart_e uart, uint8_t* buffer, uint16_t count);
	uint8_t uart_Read(Uart_e uart, uint8_t* buffer, uint16_t* count, BOOL wait);
	uint8_t moto_Read(uint8_t address, uint8_t* byte);
	uint8_t moto_Write(uint8_t address, uint8_t byte);

	/* Contrôle des lignes et des paramètres des UART */
	uint8_t uart_ModParam(Uart_e uart, uint8_t regLCR, uint8_t baud);
	uint8_t uart_ModLines(void);
	uint8_t uart_GetCount(Uart_e uart, uint16_t* count);

};
