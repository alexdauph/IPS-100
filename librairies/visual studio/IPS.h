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
	LogLevel_e code_get_code_level(uint8_t code);
	string code_get_code_level_text(uint8_t code);
	string code_get_text(uint8_t code);

	/* Transforme les parametres recus en trames génériques afin d'appeler les méthodes privées */
	uint8_t gpio_read(uint8_t address, uint8_t* byte);
	uint8_t gpio_write(uint8_t address, uint8_t byte);
	uint8_t memory_read(uint16_t address, uint8_t* buffer, uint8_t count);
	uint8_t memory_write(uint16_t address, uint8_t* buffer, uint8_t count);
	uint8_t uart3_read(uint8_t* buffer, uint16_t count);
	uint8_t uart3_read(uint8_t* buffer, uint16_t* count);
	uint8_t uart3_write(uint8_t* buffer, uint16_t count);
	uint8_t rs232_read(uint8_t* buffer, uint16_t count);
	uint8_t rs232_read(uint8_t* buffer, uint16_t* count);
	uint8_t rs232_write(uint8_t* buffer, uint16_t count);
	uint8_t motorola_read(uint8_t address, uint8_t* byte);
	uint8_t motorola_write(uint8_t address, uint8_t byte);
	uint8_t i2c_read(uint8_t address, uint8_t* buffer, uint8_t count);
	uint8_t i2c_write(uint8_t address, uint8_t* buffer, uint8_t count);

	/* Appel les méthodes privées de modification/lecture des paramètres UART et lecture du nombre d'octet en attente */
	uint8_t uart_set_param(Uart_e uart);
	uint8_t uart_get_param(Uart_e uart);
	uint8_t uart3_get_count(uint16_t* count);
	uint8_t rs232_get_count(uint16_t* count);

	/* Appel les méthodes privées de controles des lignes ou des paramètres RS232 */
	void rs232_enable_flow_control(void);
	void rs232_disable_flow_control(void);
	void rs232_enable_RTS(void);
	void rs232_enable_DTR(void);
	void rs232_disable_RTS(void);
	void rs232_disable_DTR(void);
	BOOL rs232_get_CTS(void);
	BOOL rs232_get_DCE(void);
	BOOL rs232_get_RI(void);
	BOOL rs232_get_DCD(void);

private:
	/* Overture d'un mPort COM */
	uint8_t com_connect(void);
	uint8_t com_connect(uint8_t port);

	/* Methodes de communication directe avec le LPC */
	BOOL com_wait(uint8_t breakCondition);
	uint8_t com_get_byte(void);
	BOOL com_send_byte(uint8_t byte);
	uint8_t com_get_data(uint8_t* buffer, uint8_t count);
	uint8_t com_send_data(uint8_t* buffer, uint8_t count);
	uint8_t com_request(uint8_t command, uint8_t reply, uint8_t* var);

	/* Détection et gestion d'erreurs critiques */
	uint8_t com_verify(uint8_t result);
	void error_handle(uint8_t error);

	/* Transforme les trames génériques en blocs de données */
	uint8_t uart_read(Uart_e uart, uint8_t* buffer, uint16_t* count, BOOL wait);
	uint8_t uart_write(Uart_e uart, uint8_t* buffer, uint16_t count);
	uint8_t moto_read(uint8_t address, uint8_t* byte);
	uint8_t moto_write(uint8_t address, uint8_t byte);

	/* Contrôle des lignes et des paramètres des UART */
	uint8_t uart__mod_param(Uart_e uart, uint8_t regLCR, uint8_t baud);
	uint8_t uart_mod_lines(void);
	uint8_t uart_get_count(Uart_e uart, uint16_t* count);

};
