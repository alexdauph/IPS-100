#ifndef VCOM_H_
#define VCOM_H_


#include "stdint.h"


#ifdef __cplusplus
extern "C" {
#endif

void WFD(void);
extern void UART1_IRQHandler(void);
extern void UART3_IRQHandler(void);

extern uint32_t vcom_write (uint8_t *pBuf, uint32_t buf_len);
extern uint32_t vcom_bread(uint8_t *pBuf, uint32_t buf_len);
extern uint32_t GetCount(void);

#ifdef __cplusplus
}
#endif


#define min(a,b)            (((a) < (b)) ? (a) : (b))
#define max(a,b)            (((a) > (b)) ? (a) : (b))


class VCOM {
public:
	enum Uart_e {
		RS232 = 1,
		UART3 = 3,
	};

	typedef struct {					/*!< USARTn Structure       */
		union {
			volatile uint32_t DLL;
			volatile uint32_t THR;
			volatile uint32_t RBR;
		};
		union {
			volatile uint32_t IER;
			volatile uint32_t DLM;
		};
		union {
			volatile uint32_t FCR;
			volatile uint32_t IIR;
		};
		volatile uint32_t LCR;
		volatile uint32_t MCR;
		volatile uint32_t LSR;
		volatile uint32_t MSR;
		volatile uint32_t SCR;
		volatile uint32_t ACR;
		volatile uint32_t ICR;
		volatile uint32_t FDR;
		volatile uint32_t OSR;
		volatile uint32_t TER1;

	} UartReg_t;


	struct Uart_t {
		UartReg_t* registers;
		Uart_e ID;
		uint8_t rxData[2048];
		uint8_t txData[2048];
		uint16_t index;
		uint16_t count;
		uint32_t SavedLSR;
		bool newData;
	};
	Uart_t uart1, uart3;


private:


private:
	//Initiation du système
	void init_ST(void);
	void init_I2C(void);
	void init_UART1(void);
	void init_UART3(void);
	void init_MOTOROLA(void);

	//Envoi et réception d'un octet sur le lien vcom
	uint8_t com_GetByte(void);
	bool com_SendByte(uint8_t byte);
	uint8_t com_GetData(uint8_t* buffer, uint8_t count);
	uint8_t com_SendData(uint8_t* buffer, uint8_t count);

	//Méthodes d'attente
	bool wait_VCOM(uint32_t microSeconds);
	bool wait_UART_RX(Uart_e uart, uint32_t microSeconds);
	bool wait_UART_TX(Uart_e uart, uint32_t microSeconds);
	bool wait_I2C(uint8_t val, uint32_t microSeconds);
	bool wait_MOTO(uint32_t tensNanoSeconds);

	//Lecture et écriture en I2C
	uint8_t i2c_Send(uint8_t address, uint8_t* buffer, uint8_t count);
	uint8_t i2c_Get(uint8_t address, uint8_t* buffer, uint8_t count);

public:
	VCOM();
	~VCOM();

	//Méthodes appellées par l'application
	void Init(void);
	void Ping(void);
	void i2c_Read(void);
	void i2c_Write(void);
	void uart_Read(void);
	void uart_Write(void);
	void moto_Read(void);
	void moto_Write(void);
	void uart_ModParam(void);
	void uart_ModLines(void);
	void uart_GetCount(void);

};


typedef struct {
	uint8_t rqst;
	void (VCOM::* action)(void);
} Operation_t;


#endif /* VCOM_H_ */
