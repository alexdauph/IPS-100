#pragma once
#include <iostream>
#include "TB8.h"
using namespace std;


#define MENU_MAIN				Menu::menus[0]
#define MENU_GPIO				Menu::menus[1]
#define MENU_MEMORY				Menu::menus[2]
#define MENU_UART				Menu::menus[3]
#define MENU_RS232				Menu::menus[4]
#define MENU_MOTOROLA			Menu::menus[5]
#define MENU_DEBUG				Menu::menus[6]
#define OPTION_COUNT			sizeof(options) / sizeof(OptionData_t)


struct MenuData_t;
struct OptionData_t;
struct WindowData_t;
class Window;


enum OptionType_e {
	SubMenu,
	Method_TB8,
	Method_Window,
};

struct MenuData_t {
	string name;
	OptionData_t* options;
	uint8_t optionCount;
	MenuData_t* parent;
	WindowData_t* window;
};

struct OptionData_t {
	string text;
	OptionType_e type;
	MenuData_t* subMenu;
	void (TB8::* method_tb8)(void);
	void (Window::* method_window)(void);
};


class Menu {
private:
	string name;
	uint8_t optionCount;
	MenuData_t* parent;
	uint8_t currentSel;

public:
	static MenuData_t menus[7];
	static OptionData_t main_options[6];
	static OptionData_t gpio_options[4];
	static OptionData_t memory_options[4];
	static OptionData_t uart_options[6];
	static OptionData_t rs232_options[9];
	static OptionData_t motorola_options[4];
	static OptionData_t debug_options[5];

	OptionData_t* options;
	Window* currentWindow;

private:
	
public:
	Menu(MenuData_t* data);
	~Menu();
	void Navigate(void);
};