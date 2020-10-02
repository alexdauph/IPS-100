#include <conio.h>
#include "SerialPort.h"
#include "IPS.h"
#include "codes.h"
#include "Console.h"


#define WAIT_DEBUG				0x00			//À des fins de debug seulement lors de la création du module IPS-100
#define WAIT_WITH_ESC			0x00
#define WAIT_DEFAULT			0x01
#define MAX_FRAME_COUNT			64


#define CODE_COUNT			(sizeof(debugLogs)/sizeof(debugLogs[0]))


Console console;

string LogText[] = { "DEBUG","SUCCES","AVERTISSEMENT","ERREUR","ECHEC" };

LogMessage_t debugLogs[] = {
	{COM_OK,				SUCCES,				"COM_OK",					"Port COM ouvert"},
	{COM_ERROR,				ERREUR,				"COM_ERROR",				"Impossible d'ouvrir ce port COM"},
	{COM_FAIL,				ERREUR,				"COM_FAIL",					"Impossible d'ouvrir le port COM recu par le constructeur"},

	{LPC_RQST,				SUCCES,				"LPC_RQST",					"Requete d'initialisation du LPC"},
	{LPC_PING,				SUCCES,				"LPC_PING",					"Test pour verifier la connexion avec le LPC"},
	{LPC_READY,				SUCCES,				"LPC_READY",				"LPC pret"},
	{LPC_LOST,				ERREUR,				"LPC_LOST",					"Connexion avec le LPC interrompue"},
	{LPC_ERROR,				ERREUR,				"LPC_ERROR",				"Bris de protocole entre l'application pilote et le LPC"},

	{I2C_RD,				SUCCES,				"I2C_RD",					"Lecture I2C"},
	{I2C_WR,				SUCCES,				"I2C_WR",					"Ecriture I2C"},
	{I2C_READY,				SUCCES,				"I2C_READY",				"I2C pret"},
	{I2C_ABORT,				ERREUR,				"I2C_ABORT",				"Requete I2C annulee"},
	{I2C_OK,				SUCCES,				"I2C_OK",					"Requete I2C effectuee avec succes"},
	{I2C_BAD_CNT,			ERREUR,				"I2C_BAD_CNT",				"Le LPC n'a pas envoye le bon nombre de donnees"},
	{I2C_ERROR,				ERREUR,				"I2C_ERROR",				"Erreur lors de la transmission des donnees"},
	{I2C_FAIL,				ERREUR,				"I2C_FAIL",					""},
	{I2C_TIMEOUT,			ERREUR,				"I2C_TIMEOUT",				""},
	{I2C_NACK,				ERREUR,				"I2C_NACK",					"L'esclave I2C n'a pas repondu a la requete"},
	{I2C_NO_START,			ERREUR,				"I2C_NO_START",				"Impossible de generer une START sur le bus I2C"},
	{I2C_NO_SLAVE,			ERREUR,				"I2C_NO_SLAVE",				"Aucun esclave I2C n'a repondu a cette adresse"},
	{I2C_OFLOW,				ERREUR,				"I2C_OFLOW",				"Nombre d'octets trop eleve"},
	{I2C_PTR_ERR,			ERREUR,				"I2C_PTR_ERR",				"Erreur lors de placement du pointeur interne"},

	{UART_RD,				SUCCES,				"UART_RD",					"Lecture UART"},
	{UART_WR,				SUCCES,				"UART_WR",					"Ecriture UART"},
	{UART_MOD_PARAM,		SUCCES,				"UART_MOD_PARAM",			"Parametres UART modifies"},
	{UART_MOD_LINES,		SUCCES,				"UART_MOD_LINES",			"Ligne de controles RS232 modifiee"},
	{UART_READY,			SUCCES,				"UART_READY",				"UART pret"},
	{UART_ABORT,			ERREUR,				"UART_ABORT",				"Requete UART annulee"},
	{UART_OK,				SUCCES,				"UART_OK",					"Requete UART effectuee avec succes"},
	{UART_EMPTY,			ERREUR,				"UART_EMPTY",				"Aucune donnee disponible"},
	{UART_UFLOW,			AVERTISSEMENT,		"UART_UFLOW",				"Le FIFO contenait moins de donnees que demande"},
	{UART_OFLOW,			ERREUR,				"UART_OFLOW",				"Nombre d'octets trop eleve"},
	{UART_BAD_CNT,			ERREUR,				"UART_BAD_CNT",				"Le LPC n'a pas envoye le bon nombre de donnees"},
	{UART_ERROR,			ERREUR,				"UART_ERROR",				"Erreur lors de la transmission des donnees"},
	{UART_OVERRUN,			ERREUR,				"UART_OVERRUN",				"Erreur d'ecrasement des donnees ou FIFO plein"},
	{UART_PARITY,			ERREUR,				"UART_PARITY",				"Erreur de parite"},
	{UART_FRAMING,			ERREUR,				"UART_FRAMING",				"Erreur de cadrage"},
	{UART_GET_CNT,			ERREUR,				"UART_GET_CNT",				"Requete du nombre d'octets dans le FIFO"},

	{MOTOROLA_RD,			SUCCES,				"MOTOROLA_RD",				"Lecture bus parallele"},
	{MOTOROLA_WR,			SUCCES,				"MOTOROLA_WR",				"Ecriture bus parallele"},
	{MOTOROLA_READY,		SUCCES,				"MOTOROLA_READY",			"Bus parallele pret"},
	{MOTOROLA_ABORT,		ERREUR,				"MOTOROLA_ABORT",			"Requete bus parallele annulee"},
	{MOTOROLA_OK,			SUCCES,				"MOTOROLA_OK",				"Operation effectuee avec succes"},
	{MOTOROLA_UFLOW,		AVERTISSEMENT,		"MOTOROLA_UFLOW",			""},
	{MOTOROLA_OFLOW,		ERREUR,				"MOTOROLA_OFLOW",			""},
	{MOTOROLA_BAD_CNT,		ERREUR,				"MOTOROLA_BAD_CNT",			"Le LPC n'a pas envoye le bon nombre de donnees"},
	{MOTOROLA_ERROR,		ERREUR,				"MOTOROLA_ERROR",			"Erreur lors de la transmission des donnees"},
	{MOTOROLA_FAIL,			ERREUR,				"MOTOROLA_FAIL",			"Aucune reponse sur le bus parallele"},
};


/******************************************************************************************************************/
/* Constructeurs et destructeur */
/******************************************************************************************************************/

IPS::IPS() {
	mVirtualPort = new SerialPort;

	while (!com_Connect());
}

IPS::IPS(uint8_t port) {
	mVirtualPort = new SerialPort;

	if (!com_Connect(port)) {
		console.clear_Rectangle({ 0, 3 }, { 79, 3 });
		console.io_WriteText({ 0,3 }, "Impossible d'ouvrir le port COM fourni par le constructeur", WHITE_ON_BLACK);
		while (!com_Connect());
	}
}

IPS::~IPS() {
	mVirtualPort->Close();
	delete mVirtualPort;	
}

/******************************************************************************************************************/
/* Ouverture d'un port COM */
/******************************************************************************************************************/

uint8_t IPS::com_Connect() {
	uint16_t height = console.screen_GetTop();

	while (1) {
		console.clear_Rectangle({ 0, height + 2 }, { 79, height + 2 });
		console.io_WriteText({ 0, height + 2 }, "Entrez le numero du port COM : ", WHITE_ON_BLACK);

		if (console.io_Input((uint32_t*)&mPort, 2, 10)) {
			cout << "\n\r";
			break;
		}
	}
	return com_Connect(mPort);
}

uint8_t IPS::com_Connect(uint8_t port) {
	uint8_t result;
	uint8_t byte;
	uint8_t command;
	uint16_t height = console.screen_GetTop();

	mPort = port;
	mVirtualPort->Close();
	mConnecting = true;

	if (mVirtualPort->Open(mPort, 115200)) {
		while (mVirtualPort->CheckData()) {							//Vider le buffer si présent
			mVirtualPort->Read(&byte, 1);
			Sleep(10);
		}

		if (mInitialized == false) { command = LPC_RQST; }
		else { command = LPC_PING; }

		if (com_Request(command, LPC_READY, &result)) {
			mConnecting = false;
			mInitialized = true;
			return 1;
		}
		else {
			mVirtualPort->Close();
			console.clear_Rectangle({ 0, height + 3 }, { 79, height + 3 });
			console.io_WriteText({ 0, height + 3 }, "Aucun module IPS-100 disponible sur ce port", WHITE_ON_BLACK);
			return 0;
		}
	}
	else {
		console.clear_Rectangle({ 0, height + 3 }, { 79, height + 3 });
		console.io_WriteText({ 0, height + 3 }, "Impossible d'ouvrir ce port COM", WHITE_ON_BLACK);
		return 0;
	}
}

/******************************************************************************************************************/
/* Methodes de communication directe avec le LPC */
/******************************************************************************************************************/

uint8_t IPS::com_GetByte(void) {
	com_Wait(WAIT_DEFAULT);
	char buffer[1];
	mVirtualPort->Read(buffer, 1);
	return buffer[0];
}

BOOL IPS::com_SendByte(uint8_t byte) {
	char buffer[1] = { byte };
	return mVirtualPort->Write(buffer, 1) == 1;
}

uint8_t IPS::com_GetData(uint8_t* buffer, uint8_t count) {
	com_Wait(WAIT_DEFAULT);
	return mVirtualPort->Read(buffer, count);
}

uint8_t IPS::com_SendData(uint8_t* buffer, uint8_t count) {
	return mVirtualPort->Write((char*)buffer, count);
}

uint8_t IPS::com_Request(uint8_t command, uint8_t reply, uint8_t* var) {
	com_SendByte(command);
	*var = com_GetByte();
	return *var == reply;
}

BOOL IPS::com_Wait(uint8_t breakCondition) {
	uint16_t timeOutCounter = 0;

	while (!mVirtualPort->CheckData()) {
		#if(WAIT_DEBUG==0)
		if (breakCondition == WAIT_DEFAULT) {						//Détection de la perte de connexion avec le IPS-100
			timeOutCounter++;
			if (timeOutCounter > 1500) {							//Éviter la récursivité de la méthode "IPS::com_Connect(uint8_t port)"
				if (mConnecting == true) { return false; }			//Erreur de prise de contact. Aucun module IPS-100 disponible sur ce port
				else { throw (uint8_t)LPC_ERROR; }					//Bris de protocole
			}
		}
		else if (breakCondition == WAIT_WITH_ESC && _kbhit()) {
			if (console.io_Getch() == KEY_ESC) {					//Utilisé dans les méthodes où on attend des données pour une durée indéterminée
				com_SendByte(0x55);
				return false;
			}
		}
		#else 
		if (breakCondition == WAIT_WITH_ESC && _kbhit()) {
			if (console.io_Getch() == KEY_ESC) {					//Utilisé dans les méthodes où on attend des données pour une durée indéterminée
				com_SendByte(0x55);
				return false;
			}
		}
		#endif											
		Sleep(1);
	}

	return true;
}

/******************************************************************************************************************/
/* Détection et gestion d'erreurs critiques */
/******************************************************************************************************************/

uint8_t IPS::com_Verify(uint8_t result) {
	for (uint8_t i = 0; i < CODE_COUNT; i++) {
		if (result == debugLogs[i].ID) {
			return result;
		}
	}

	mVirtualPort->Close();
	if (mVirtualPort->Open(mPort, 115200)) {
		throw (uint8_t)LPC_ERROR;									//Bris de protocole
	}
	else {
		throw (uint8_t)LPC_LOST;									//LPC déconnecté
	}
}

void IPS::ErrorHandler(uint8_t error) {
	uint16_t height = console.screen_GetTop();

	console.screen_Save();
	console.screen_Clear();
	if (error == LPC_LOST)
		console.io_WriteText({ 17, height + 12 }, "ERREUR CRITIQUE : PERTE DE CONTACT AVEC LE LPC", YELLOW_ON_BLACK);
	else if (error == LPC_ERROR)
		console.io_WriteText({ 22, height + 12 }, "ERREUR CRITIQUE : BRIS DE PROTOCOLE", YELLOW_ON_BLACK);

	while (!com_Connect());
	console.screen_Print();
}


/******************************************************************************************************************/
/* Information par rapport au code de résultat */
/******************************************************************************************************************/

LogLevel_e IPS::GetCodeLevel(uint8_t code) {
	for (uint8_t i = 0; i < CODE_COUNT; i++) {
		if (code == debugLogs[i].ID) { return debugLogs[i].level; }
	}

	return INVALIDE;
}

string IPS::GetCodeLevelText(uint8_t code) {
	for (uint8_t i = 0; i < CODE_COUNT; i++) {
		if (code == debugLogs[i].ID) {
			return ("[" + LogText[debugLogs[i].level] + "] " + console.io_IntToHex(code, 2)) + " - " + debugLogs[i].name;
		}
	}

	return "[INVALIDE]";
}

string IPS::GetCodeText(uint8_t code) {
	for (uint8_t i = 0; i < CODE_COUNT; i++) {
		if (code == debugLogs[i].ID) {
			return debugLogs[i].text;
		}
	}

	return "Une erreur s'est produite et ce code n'est pas valide";
}


/******************************************************************************************************************/
/* Transforme les parametres recus en trames génériques afin d'appeler les méthodes privées */
/******************************************************************************************************************/

uint8_t IPS::gpio_Read(uint8_t address, uint8_t* byte) {
	return i2c_Read(address, byte, 1);
}

uint8_t IPS::gpio_Write(uint8_t address, uint8_t byte) {
	return i2c_Write(address, &byte, 1);
}

uint8_t IPS::memory_Read(uint16_t address, uint8_t* buffer, uint8_t count) {
	uint8_t result;
	uint8_t pointerBuffer[] = { ((address & 0xFF00) >> 8), ((address & 0x00FF) >> 0) };

	if (count > 256) { return I2C_OFLOW; }							//Nombre d'octet maximum (arbitraire) d'une écriture I2C

	result = i2c_Write(0xA0, pointerBuffer, 2);						//Place le pointeur interne de la memoire EEPROM

	if (result == I2C_OK)
		result = i2c_Read(0xA0, buffer, count);
	else
		result = I2C_PTR_ERR;										//Erreur lors de placement du pointeur interne						

	return result;
}

uint8_t IPS::memory_Write(uint16_t address, uint8_t* buffer, uint8_t count) {
	uint8_t result;
	uint8_t newBuffer[66];											//Besoin d'un buffer avec 2 octets de plus que le maximum

	if (count > 64) { return I2C_OFLOW; }							//Nombre d'octet maximum sur une "page" du 24LC256

	newBuffer[0] = (address & 0xFF00) >> 8;							//Ajouter l'addresse memoire a la trame
	newBuffer[1] = (address & 0x00FF) >> 0;
	for (uint8_t i = 0; i < count; i++) {							//Copier le reste du data après l'addresse
		newBuffer[i + 2] = buffer[i];
	}

	result = i2c_Write(0xA0, newBuffer, count + 2);

	return result;
}

uint8_t IPS::uart3_Read(uint8_t* buffer, uint16_t count) {
	return uart_Read(UART3, buffer, &count, false);					//False pour indiquer au LPC de ne pas attendre les données
}

uint8_t IPS::uart3_Read(uint8_t* buffer, uint16_t* count) {
	return uart_Read(UART3, buffer, count, true);					//True pour indiquer au LPC d'attendre les données indéfiniment 
}

uint8_t IPS::uart3_Write(uint8_t* buffer, uint16_t count) {
	return uart_Write(UART3, buffer, count);						//UART3 ajouté pour indiquer sur quel uart effectuer l'instruction
}

uint8_t IPS::rs232_Read(uint8_t* buffer, uint16_t count) {
	return uart_Read(RS232, buffer, &count, false);					//False pour indiquer au LPC de ne pas attendre les données
}

uint8_t IPS::rs232_Read(uint8_t* buffer, uint16_t* count) {
	return uart_Read(RS232, buffer, count, true);					//True pour indiquer au LPC d'attendre les données indéfiniment 
}

uint8_t IPS::rs232_Write(uint8_t* buffer, uint16_t count) {
	return uart_Write(RS232, buffer, count);						//RS232 ajouté pour indiquer sur quel uart effectuer l'instruction
}

uint8_t IPS::motorola_Read(uint8_t address, uint8_t* byte) {
	return moto_Read(address, byte);								//Ne requiert aucune modification
}

uint8_t IPS::motorola_Write(uint8_t address, uint8_t byte) {
	return moto_Write(address, byte);								//Ne requiert aucune modification
}


/******************************************************************************************************************/
/* Appel les méthodes privées de modification/lecture des paramètres UART et lecture du nombre d'octet en attente */
/******************************************************************************************************************/

uint8_t IPS::uart_SetParam(Uart_e uart) {
	const string bitRate[8] = { "300", "1200", "2400", "4800", "9600", "19200", "38400", "115200" };
	const string bitCount[4] = { "5", "6", "7", "8" };
	const string stopBits[2] = { "1", "2" };
	const string parity[5] = { "Aucune", "Impaire", "Paire", "Mark", "Space" };
	const uint8_t bitRateVals[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
	const uint8_t bitCountVals[4] = { 0x00, 0x01, 0x02, 0x03 };
	const uint8_t stopBitsVals[2] = { 0x00, 0x04 };
	const uint8_t parityVals[5] = { 0x00, 0x8, 0x18, 0x28, 0x38 };

	COORD coord = console.cursor_GetPosition();
	uint8_t oldBaudRate, newBaudRate = 0x00;
	uint8_t oldRegLCR, newRegLCR = 0x00;
	uint8_t var;

	if (uart == RS232) {
		oldBaudRate = mRs232Baud;
		oldRegLCR = mRs232LCR;
	}
	else if (uart == UART3) {
		oldBaudRate = mUart3Baud;
		oldRegLCR = mUart3LCR;
	}

	console.io_WriteText(coord, "Debit binaire : ", WHITE_ON_BLACK);
	if (console.io_InputCycle(bitRate, bitRateVals, &newBaudRate, oldBaudRate, 8)) {

		coord.Y++;
		console.io_WriteText(coord, "Bits par mot : ", WHITE_ON_BLACK);
		if (console.io_InputCycle(bitCount, bitCountVals, &var, oldRegLCR & MASK_BITCOUNT, 4)) {

			newRegLCR |= var;
			coord.Y++;
			console.io_WriteText(coord, "Bits d'arret : ", WHITE_ON_BLACK);
			if (console.io_InputCycle(stopBits, stopBitsVals, &var, oldRegLCR & MASK_STOPBITS, 2)) {

				newRegLCR |= var;
				coord.Y++;
				console.io_WriteText(coord, "Partite : ", WHITE_ON_BLACK);
				if (console.io_InputCycle(parity, parityVals, &var, oldRegLCR & MASK_PARITY, 5)) {

					newRegLCR |= var;
					uart_ModParam(uart, newRegLCR, newBaudRate);
					return 1;
				}
			}
		}
	}

	return 0;
}

uint8_t IPS::uart_GetParam(Uart_e uart) {
	const string bitRate[8] = { "300", "1200", "2400", "4800", "9600", "19200", "38400", "115200" };
	const string bitCount[4] = { "5", "6", "7", "8" };
	const string stopBits[2] = { "1", "2" };
	const string parity[5] = { "Aucune", "Impaire", "Paire", "Mark", "Space" };
	const uint8_t bitRateVals[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
	const uint8_t bitCountVals[4] = { 0x00, 0x01, 0x02, 0x03 };
	const uint8_t stopBitsVals[2] = { 0x00, 0x04 };
	const uint8_t parityVals[5] = { 0x00, 0x8, 0x18, 0x28, 0x38 };

	COORD coord = console.cursor_GetPosition();
	uint8_t oldBaudRate;
	uint8_t oldRegLCR;
	uint8_t var;

	if (uart == RS232) {
		oldBaudRate = mRs232Baud;
		oldRegLCR = mRs232LCR;
	}
	else if (uart == UART3) {
		oldBaudRate = mUart3Baud;
		oldRegLCR = mUart3LCR;
	}

	for (uint8_t i = 0; i < 8; i++) {
		if (oldBaudRate == bitRateVals[i]) {
			console.io_WriteText(coord, "Debit binaire : " + bitRate[i], WHITE_ON_BLACK);
			break;
		}
	}

	coord.Y++;
	for (uint8_t i = 0; i < 4; i++) {
		if ((oldRegLCR & MASK_BITCOUNT) == bitCountVals[i]) {
			console.io_WriteText(coord, "Bits par mot : " + bitCount[i], WHITE_ON_BLACK);
			break;
		}
	}

	coord.Y++;
	for (uint8_t i = 0; i < 2; i++) {
		if ((oldRegLCR & MASK_STOPBITS) == stopBitsVals[i]) {
			console.io_WriteText(coord, "Bits d'arret : " + stopBits[i], WHITE_ON_BLACK);
			break;
		}
	}

	coord.Y++;
	for (uint8_t i = 0; i < 5; i++) {
		if ((oldRegLCR & MASK_PARITY) == parityVals[i]) {
			console.io_WriteText(coord, "Partite : " + parity[i], WHITE_ON_BLACK);
			break;
		}
	}

	return 0;
}

uint8_t IPS::uart3_GetCount(uint16_t* count) {
	return uart_GetCount(UART3, count);
}

uint8_t IPS::rs232_GetCount(uint16_t* count) {
	return uart_GetCount(RS232, count);
}


/******************************************************************************************************************/
/* Appel les méthodes privées de controles des lignes ou des paramètres UART */
/******************************************************************************************************************/

void IPS::rs232_EnableFlowControl(void) {
	mRs232Lines |= 0x04;
	uart_ModLines();
}

void IPS::rs232_DisableFlowControl(void) {
	mRs232Lines &= ~0x04;
	uart_ModLines();
}

void IPS::rs232_EnableRTS(void) {
	mRs232Lines |= 0x02;
	uart_ModLines();
}
void IPS::rs232_EnableDTR(void) {
	mRs232Lines |= 0x01;
	uart_ModLines();
}
void IPS::rs232_DisableRTS(void) {
	mRs232Lines &= ~0x02;
	uart_ModLines();
}
void IPS::rs232_DisableDTR(void) {
	mRs232Lines &= ~0x01;
	uart_ModLines();
}

BOOL IPS::rs232_GetCTS(void) {
	uart_ModLines();
	return mRs232Lines & 0x10;
}

BOOL IPS::rs232_GetDCE(void) {
	uart_ModLines();
	return mRs232Lines & 0x20;
}

BOOL IPS::rs232_GetRI(void) {
	uart_ModLines();
	return mRs232Lines & 0x40;
}

BOOL IPS::rs232_GetDCD(void) {
	uart_ModLines();
	return mRs232Lines & 0x80;
}


/******************************************************************************************************************/
/* Transforme les trames génériques en blocs de données */
/******************************************************************************************************************/

uint8_t IPS::i2c_Read(uint8_t address, uint8_t* buffer, uint8_t count) {
	uint8_t header[] = { address, count };
	uint8_t result;

	try {
		if (count == 0) { return I2C_OK; }
		if (count > 256) { return I2C_OFLOW; }

		if (com_Request(I2C_RD, I2C_READY, &result)) {
			com_SendData(header, 2);								//Envoyer l'en-tête de trame I2C

			if (com_Wait(WAIT_DEFAULT)) {							//press esc to abort
				result = com_GetByte();

				if (GetCodeLevel(result) <= AVERTISSEMENT) {		//I2C_OK ou I2C_UFLOW
					for (uint16_t i = 0; i < count; i += 32) {
						com_SendByte(0x01);							//Indiquer au LPC que l'application est prête a recevoir des nouveaux octets

						if (com_GetData(&buffer[i], min(count - i, 32)) != min(count - i, 32)) {
							result = I2C_BAD_CNT;					//Erreur d'execution
							break;
						}
					}
				}
			}
			else { result = I2C_ABORT; }
		}

		return com_Verify(result);
	}
	catch (uint8_t error) {
		ErrorHandler(error);
		return error;
	}
}

uint8_t IPS::i2c_Write(uint8_t address, uint8_t* buffer, uint8_t count) {
	uint8_t header[] = { address, count };
	uint8_t result;

	try {
		if (count == 0) { return I2C_OK; }
		if (count > 256) { return I2C_OFLOW; }

		if (com_Request(I2C_WR, I2C_READY, &result)) {
			com_SendData(header, 2);								//Envoyer l'en-tête de trame UART

			for (uint16_t i = 0; i < count; i += 32) {
				if (com_GetByte() == 0x01) {
					com_SendData(&buffer[i], min(count - i, 32));	//Envoyer des octets
				}
				else {
					result = I2C_BAD_CNT;							//Erreur d'execution
					break;
				}
			}

			com_Wait(WAIT_WITH_ESC);								//press esc to abort
			result = com_GetByte();									//Recevoir le résultat
		}

		return com_Verify(result);
	}
	catch (uint8_t error) {
		ErrorHandler(error);
		return error;
	}
}

uint8_t IPS::uart_Read(Uart_e uart, uint8_t* buffer, uint16_t* count, BOOL wait) {
	uint8_t header[] = { uart, (*count & 0x00FF) >> 0, (*count & 0xFF00) >> 8, wait };
	uint8_t result;

	try {
		if (*count == 0) { return UART_OK; }
		if (*count > 2048) { return UART_OFLOW; }

		if (com_Request(UART_RD, UART_READY, &result)) {
			com_SendData(header, 4);								//Envoyer l'en-tête de trame UART

			if (com_Wait(WAIT_WITH_ESC)) {							//press esc to abort
				com_GetData(header, 3);
				result = header[0];
				*count = (header[1] << 0) | (header[2] << 8);

				if (GetCodeLevel(result) <= AVERTISSEMENT) {		//UART_OK ou UART_UFLOW
					for (uint16_t i = 0; i < *count; i += 32) {
						com_SendByte(0x01);							//Indiquer au LPC que l'application est prête a recevoir des nouveaux octets

						if (com_GetData(&buffer[i], min(*count - i, 32)) != min(*count - i, 32)) {
							result = UART_BAD_CNT;					//Erreur d'execution
							break;
						}
					}
				}
			}
			else { result = UART_ABORT; }
		}

		return com_Verify(result);
	}
	catch (uint8_t error) {
		ErrorHandler(error);
		return error;
	}
}

uint8_t IPS::uart_Write(Uart_e uart, uint8_t* buffer, uint16_t count) {
	uint8_t header[] = { uart, (count & 0x00FF) >> 0, (count & 0xFF00) >> 8 };
	uint8_t result;

	try {
		if (count == 0) { return UART_OK; }
		if (count > 2048) { return UART_OFLOW; }

		if (com_Request(UART_WR, UART_READY, &result)) {
			com_SendData(header, 3);								//Envoyer l'en-tête de trame UART

			for (uint16_t i = 0; i < count; i += 32) {
				if (com_GetByte() == 0x01) {						//Attendre que le LPC soit prêt à recevoir des nouveaux octets
					com_SendData(&buffer[i], min(count - i, 32));	//Envoyer des octets
				}
				else {
					result = UART_BAD_CNT;							//Erreur d'execution
					break;
				}
			}

			com_Wait(WAIT_WITH_ESC);								//press esc to abort
			result = com_GetByte();									//Recevoir le résultat
		}

		return com_Verify(result);
	}
	catch (uint8_t error) {
		ErrorHandler(error);
		return error;
	}
}

uint8_t IPS::moto_Read(uint8_t address, uint8_t* byte) {
	uint8_t data[] = { address };
	uint8_t result;

	try {
		if (com_Request(MOTOROLA_RD, MOTOROLA_READY, &result)) {

			com_SendData(data, 1);									//Envoyer l'adresse à lire

			if (com_GetData(byte, 1) == 1) {						//Lire l'octet
				result = com_GetByte();								//Recevoir le résultat
			}
			else {
				result = MOTOROLA_BAD_CNT;							//Erreur d'execution
			}
		}

		return com_Verify(result);
	}
	catch (uint8_t error) {
		ErrorHandler(error);
		return error;
	}
}

uint8_t IPS::moto_Write(uint8_t address, uint8_t byte) {
	uint8_t data[] = { address, byte };
	uint8_t result;

	try {
		if (com_Request(MOTOROLA_WR, MOTOROLA_READY, &result)) {

			com_SendData(data, 2);									//Envoyer l'adresse à lire

			if (com_GetData(&byte, 1) == 1) {						//Lire l'octet
				result = com_GetByte();								//Recevoir le résultat
			}
			else {
				result = MOTOROLA_BAD_CNT;							//Erreur d'execution
			}
		}

		return com_Verify(result);
	}
	catch (uint8_t error) {
		ErrorHandler(error);
		return error;
	}
}


/******************************************************************************************************************/
/* Contrôle des lignes et parametres des UART */
/******************************************************************************************************************/

uint8_t IPS::uart_ModParam(Uart_e uart, uint8_t regLCR, uint8_t baud) {
	uint8_t data[] = { uart, regLCR, baud };
	uint8_t result;

	try {
		if (com_Request(UART_MOD_PARAM, UART_READY, &result)) {
			com_SendData(data, 3);

			result = com_GetByte();

			if (uart == RS232) {
				mRs232LCR = regLCR;
				mRs232Baud = baud;
			}
			else if (uart == UART3) {
				mUart3LCR = regLCR;
				mUart3Baud = baud;
			}
		}

		return com_Verify(result);
	}
	catch (uint8_t error) {
		ErrorHandler(error);
		return error;
	}
}

uint8_t IPS::uart_ModLines(void) {
	uint8_t data[] = { mRs232Lines };
	uint8_t result;

	try {
		if (com_Request(UART_MOD_LINES, UART_READY, &result)) {
			com_SendData(data, 1);

			mRs232Lines = com_GetByte();
			result = com_GetByte();
		}

		return com_Verify(result);
	}
	catch (uint8_t error) {
		ErrorHandler(error);
		return error;
	}
}

uint8_t IPS::uart_GetCount(Uart_e uart, uint16_t* count) {
	uint8_t data[] = { uart };
	uint8_t result;

	try {
		if (com_Request(UART_GET_CNT, UART_READY, &result)) {
			com_SendData(data, 1);

			*count = com_GetByte() << 0;
			*count |= com_GetByte() << 8;
			result = com_GetByte();
		}

		return com_Verify(result);
	}
	catch (uint8_t error) {
		ErrorHandler(error);
		return error;
	}
}