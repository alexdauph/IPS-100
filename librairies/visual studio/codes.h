#pragma once


#define COM_OK					0x00			
#define COM_ERROR				0x01			
#define COM_FAIL				0x02

#define LPC_RQST				0x10
#define LPC_PING				0x11
#define LPC_READY				0x11
#define LPC_LOST				0x12
#define LPC_ERROR				0x13					

#define I2C_RD					0x20
#define I2C_WR					0x21
#define I2C_READY				0x22
#define I2C_ABORT				0x23
#define I2C_OK					0x24
#define I2C_BAD_CNT				0x25
#define I2C_ERROR				0x26
#define I2C_FAIL				0x27
#define I2C_TIMEOUT				0x28
#define I2C_NACK				0x29
#define I2C_NO_START			0x2A
#define I2C_NO_SLAVE			0x2B
#define I2C_OFLOW				0x2C
#define I2C_PTR_ERR				0x2D

#define UART_RD					0x30
#define UART_WR					0x31
#define UART_MOD_PARAM			0x32
#define UART_MOD_LINES			0x33
#define UART_READY				0x34
#define UART_ABORT				0x35
#define UART_OK					0x36
#define UART_EMPTY				0x37
#define UART_UFLOW				0x38
#define UART_OFLOW				0x39
#define UART_BAD_CNT			0x3A
#define UART_ERROR				0x3B
#define UART_OVERRUN			0x3C
#define UART_PARITY				0x3D
#define UART_FRAMING			0x3E
#define UART_GET_CNT			0x3F

#define MOTOROLA_RD				0x40
#define MOTOROLA_WR				0x41
#define MOTOROLA_READY			0x44
#define MOTOROLA_ABORT			0x45
#define MOTOROLA_OK				0x46
#define MOTOROLA_UFLOW			0x48
#define MOTOROLA_OFLOW			0x49
#define MOTOROLA_BAD_CNT		0x4A
#define MOTOROLA_ERROR			0x4B
#define MOTOROLA_FAIL			0x4C