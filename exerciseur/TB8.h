#pragma once
#include <iostream>
#include <sstream>
using namespace std;


#define BASE_DEC			10
#define BASE_HEX			16
#define IS_CARACTER(key)	key > 0x1F && key < 0x7F


struct MenuData_t;
class Menu;
class Window;


class TB8 {
private:

public:
	Menu* currentMenu;
	MenuData_t* currentMenuData_t;

private:
	void BuildLog(string beg, uint8_t result, uint8_t* data, uint8_t count);

public:
	TB8();
	~TB8();
	void empty(void);
	void loop(void);

	void gpio_read(void);
	void gpio_write(void);
	void gpio_Debug(void);
	void memory_read(void);
	void memory_write(void);
	void memory_Debug(void);
	void uart3_read(void);
	void uart3_write(void);
	void uart3_get_count(void);
	void uart3_Debug(void);
	void rs232_read(void);
	void rs232_write(void);
	void rs232_get_count(void);
	void rs232_Debug(void);
	void motorola_read(void);
	void motorola_write(void);
	void motorola_Debug(void);

	void rs232_set_RTS(void);
	void rs232_set_DTR(void);
	void rs232_set_flow_control(void);
	void rs232_mod_param(void);
	void uart_mod_param(void);
};