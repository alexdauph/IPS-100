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
	void ST_init(void);
	void i2c_init(void);
	void uart1_init(void);
	void uart3_init(void);
	void motorola_init(void);

	//Envoi et réception d'un octet sur le lien vcom
	uint8_t com_get_byte(void);
	bool com_send_byte(uint8_t byte);
	uint8_t com_get_data(uint8_t* buffer, uint8_t count);
	uint8_t com_send_data(uint8_t* buffer, uint8_t count);

	//Méthodes d'attente
	bool wait_vcom(uint32_t microSeconds);
	bool wait_uart_rx(Uart_e uart, uint32_t microSeconds);
	bool wait_uart_tx(Uart_e uart, uint32_t microSeconds);
	bool wait_i2c(uint8_t val, uint32_t microSeconds);
	bool wait_moto(uint32_t tensNanoSeconds);

	//Lecture et écriture en I2C
	uint8_t i2c_send(uint8_t address, uint8_t* buffer, uint8_t count);
	uint8_t i2c_get(uint8_t address, uint8_t* buffer, uint8_t count);

public:
	VCOM();
	~VCOM();

	//Méthodes appellées par l'application
	void init(void);
	void ping(void);
	void i2c_read(void);
	void i2c_write(void);
	void uart_read(void);
	void uart_write(void);
	void moto_read(void);
	void moto_write(void);
	void uart_mod_param(void);
	void uart_mod_lines(void);
	void uart_get_count(void);

};


typedef struct {
	uint8_t rqst;
	void (VCOM::* action)(void);
} Operation_t;


#endif /* VCOM_H_ */
