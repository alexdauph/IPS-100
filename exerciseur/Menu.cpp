#include "Menu.h"
#include "TB8.h"
#include "Console.h"
#include "Window.h"


extern TB8 tb8;
extern Console console;


MenuData_t Menu::menus[] = {
	{" Menu Principal ",			main_options,		sizeof(main_options)	/ sizeof(OptionData_t),		&MENU_MAIN,		&WINDOW_NONE},
	{" Controle des GPIO ",			gpio_options,		sizeof(gpio_options)	/ sizeof(OptionData_t),		&MENU_MAIN,		&WINDOW_STD},
	{" Memoire 256K ",				memory_options,		sizeof(memory_options)	/ sizeof(OptionData_t),		&MENU_MAIN,		&WINDOW_STD},
	{" Communication UART3 ",		uart_options,		sizeof(uart_options)	/ sizeof(OptionData_t),		&MENU_MAIN,		&WINDOW_STD},
	{" Communication RS232 ",		rs232_options,		sizeof(rs232_options)	/ sizeof(OptionData_t),		&MENU_MAIN,		&WINDOW_STD},
	{" Communication MOTOROLA ",	motorola_options,	sizeof(motorola_options) / sizeof(OptionData_t),	&MENU_MAIN,		&WINDOW_STD},
	{" Liste des tests ",			debug_options,		sizeof(debug_options) / sizeof(OptionData_t),		&MENU_MAIN,		&WINDOW_NONE},
};

OptionData_t Menu::main_options[] = {
	{"GPIO",				SubMenu,		&MENU_GPIO,			&TB8::empty,				&Window::empty},
	{"Memoire",				SubMenu,		&MENU_MEMORY,		&TB8::empty,				&Window::empty},
	{"UART",				SubMenu,		&MENU_UART,			&TB8::empty,				&Window::empty},
	{"RS232",				SubMenu,		&MENU_RS232,		&TB8::empty,				&Window::empty},
	{"MOTOROLA",			SubMenu,		&MENU_MOTOROLA,		&TB8::empty,				&Window::empty},
	{"DEBUG",				SubMenu,		&MENU_DEBUG,		&TB8::empty,				&Window::empty},
};
OptionData_t Menu::gpio_options[] = {
	{"Lecture",				Method_TB8,		&MENU_MAIN,			&TB8::gpio_read,			&Window::empty},
	{"Ecriture",			Method_TB8,		&MENU_MAIN,			&TB8::gpio_write,			&Window::empty},
	{"Historique",			Method_Window,	&MENU_MAIN,			&TB8::empty,				&Window::Navigate},
	{"Effacer",				Method_Window,	&MENU_MAIN,			&TB8::empty,				&Window::Clear},
};
OptionData_t Menu::memory_options[] = {
	{"Lecture",				Method_TB8,		&MENU_MAIN,			&TB8::memory_read,			&Window::empty},
	{"Ecriture",			Method_TB8,		&MENU_MAIN,			&TB8::memory_write,			&Window::empty},
	{"Historique",			Method_Window,	&MENU_MAIN,			&TB8::empty,				&Window::Navigate},
	{"Effacer",				Method_Window,	&MENU_MAIN,			&TB8::empty,				&Window::Clear},
};
OptionData_t Menu::uart_options[] = {
	{"Lecture",				Method_TB8,		&MENU_MAIN,			&TB8::uart3_read,			&Window::empty},
	{"Ecriture",			Method_TB8,		&MENU_MAIN,			&TB8::uart3_write,			&Window::empty},
	{"Historique",			Method_Window,	&MENU_MAIN,			&TB8::empty,				&Window::Navigate},
	{"Effacer",				Method_Window,	&MENU_MAIN,			&TB8::empty,				&Window::Clear},
	{"Donnees en attente",	Method_TB8,		&MENU_MAIN,			&TB8::uart3_get_count,		&Window::empty},
	{"Parametres",			Method_TB8,		&MENU_MAIN,			&TB8::uart_mod_param,		&Window::empty},
};
OptionData_t Menu::rs232_options[] = {
	{"Lecture",				Method_TB8,		&MENU_MAIN,			&TB8::rs232_read,			&Window::empty},
	{"Ecriture",			Method_TB8,		&MENU_MAIN,			&TB8::rs232_write,			&Window::empty},
	{"Historique",			Method_Window,	&MENU_MAIN,			&TB8::empty,				&Window::Navigate},
	{"Effacer",				Method_Window,	&MENU_MAIN,			&TB8::empty,				&Window::Clear},
	{"Donnees en attente",	Method_TB8,		&MENU_MAIN,			&TB8::rs232_get_count,		&Window::empty},
	{"Ligne DTR",			Method_TB8,		&MENU_MAIN,			&TB8::rs232_set_DTR,			&Window::empty},
	{"Ligne RTS",			Method_TB8,		&MENU_MAIN,			&TB8::rs232_set_RTS,			&Window::empty},
	{"Flow CTS-RTS auto",	Method_TB8,		&MENU_MAIN,			&TB8::rs232_set_flow_control,	&Window::empty},
	{"Parametres",			Method_TB8,		&MENU_MAIN,			&TB8::rs232_mod_param,		&Window::empty},
};
OptionData_t Menu::motorola_options[] = {
	{"Lecture",				Method_TB8,		&MENU_MAIN,			&TB8::motorola_read,		&Window::empty},
	{"Ecriture",			Method_TB8,		&MENU_MAIN,			&TB8::motorola_write,		&Window::empty},
	{"Historique",			Method_Window,	&MENU_MAIN,			&TB8::empty,				&Window::Navigate},
	{"Effacer",				Method_Window,	&MENU_MAIN,			&TB8::empty,				&Window::Clear},
};
OptionData_t Menu::debug_options[] = {
	{"GPIO",				Method_TB8,		&MENU_MAIN,			&TB8::gpio_Debug,			&Window::empty},
	{"Memoire",				Method_TB8,		&MENU_MAIN,			&TB8::memory_Debug,			&Window::empty},
	{"UART",				Method_TB8,		&MENU_MAIN,			&TB8::uart3_Debug,			&Window::empty},
};


Menu::Menu(MenuData_t* data)
	: name(data->name),
	options(data->options),
	optionCount(data->optionCount),
	parent(data->parent),
	currentSel(0)
{
	system("cls");
	console.cursor_set_visibility(false);

	console.io_write_text(COORD_TITLE, name, BLACK_ON_WHITE);

	for (uint8_t i = 0; i < optionCount; i++) {							
		uint8_t color;

		if (i == 0) { color = BLACK_ON_WHITE; }
		else { color = WHITE_ON_BLACK; }
		
		console.io_write_text({ COORD_OPTIONS.X, COORD_OPTIONS.Y + i }, options[i].text, color);
	}

	currentWindow = new Window(data->window->coordTL, data->window->coordBR, Window::LOG);
}

Menu::~Menu() {
	delete currentWindow;
}

void Menu::Navigate(void) {
	COORD coord = COORD_OPTIONS;

	while (1) {
		string text = options[currentSel].text;
		uint8_t key = console.io_getch();

		if (key == KEY_UP && currentSel > 0) {					
			console.io_write_text(coord, text, WHITE_ON_BLACK);
			currentSel--;
			coord.Y--;
			text = options[currentSel].text;
			console.io_write_text(coord, text, BLACK_ON_WHITE);
		}
		else if (key == KEY_DOWN && currentSel < optionCount - 1) {
			console.io_write_text(coord, text, WHITE_ON_BLACK);
			currentSel++;
			coord.Y++;
			text = options[currentSel].text;
			console.io_write_text(coord, text, BLACK_ON_WHITE);
		}
		else if (key == KEY_BACK || key == KEY_ESC) {												
			tb8.currentMenuData_t = parent;
			return;
		}
		else if (key == KEY_ENTER && options[currentSel].type == SubMenu) {		
			tb8.currentMenuData_t = options[currentSel].subMenu;
			return;																	//Refresh console
		}
		else if (key == KEY_ENTER) {
			console.io_write_text(coord, text, BLACK_ON_GREY);
			console.clear_rectangle(COORD_DEBUG_TL, COORD_DEBUG_BR);
			
			if (options[currentSel].type == Method_Window) {
				void (Window:: * method)(void) = options[currentSel].method_window;
				(currentWindow->*method)();
			}
			else if (options[currentSel].type == Method_TB8) {
				(tb8.*options[currentSel].method_tb8)();
			}

			console.io_write_text(coord, text, BLACK_ON_WHITE);
			console.clear_rectangle(COORD_INPUT_TL, COORD_INPUT_BR);
			console.clear_rectangle(COORD_INPUT_TL, COORD_INPUT_BR);
			console.clear_rectangle(COORD_INPUT_TL, COORD_INPUT_BR);
		}
	}
}

