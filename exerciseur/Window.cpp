#include <iostream>
#include <string>
#include "Window.h"
#include "Console.h"
#include "TB8.h"
#include "IPS.h"


extern IPS ips;
extern Console console;
extern TB8 tb8;


const COORD COORD_TITLE				= { 5, 2 };		
const COORD COORD_DEBUG_LEVEL		= { 3, 22 };
const COORD COORD_DEBUG_TEXT		= { 3, 23 };
const COORD COORD_DEBUG_TL			= { 0, 22 };
const COORD COORD_DEBUG_BR			= { 79, 24 };
const COORD COORD_OPTIONS			= { 5,6 };
const COORD COORD_INPUT_1			= { 5, 16 };
const COORD COORD_INPUT_2			= { 5, 17 };
const COORD COORD_INPUT_TL			= { 0, 16 };
const COORD COORD_INPUT_BR			= { 79, 24 };
const COORD COORD_WINDOW_STD_TL		= { 32, 2 };
const COORD COORD_WINDOW_STD_BR		= { 73, 14 };
const COORD COORD_WINDOW_NONE_TL	= { 0, 0 };
const COORD COORD_WINDOW_NONE_BR	= { 0, 0 };



WindowData_t Window::windows[] = {
	{COORD_WINDOW_STD_TL, COORD_WINDOW_STD_BR},
	{COORD_WINDOW_NONE_TL, COORD_WINDOW_NONE_BR},
};

Window::Window(COORD coordTL, COORD coordBR, WindowType tType)
	: type(tType),
	coordTL(coordTL),
	coordBR(coordBR)
{
	console.draw_window(this);
}

Window::~Window() {

}

void Window::empty(void) {}

void Window::Log(WindowLog tLog) {
	logs.push_back(tLog);
	Update();
}

void Window::Update(void) {
	COORD coord = coordTL;
	coord.Y = coordBR.Y;

	console.clear_rectangle(coordTL, coordBR);

	for (uint16_t i = 0; i < min((coordBR.Y - coordTL.Y) + 1, logs.size()); i++) {
		uint16_t position = logs.size() - i - 1;
		uint8_t color;

		coord.Y = (coordBR.Y - i);
		
		if (type == LOG) {
			if (logs[position].level == SUCCES) { color = GREEN_ON_BLACK; }
			else if (logs[position].level == AVERTISSEMENT) { color = YELLOW_ON_BLACK; }
			else if (logs[position].level == ERREUR) { color = RED_ON_BLACK; }
			else { color = WHITE_ON_BLACK; }

			console.io_write_text(coord, logs[position].strText, color);
			console.io_write_text({ coordBR.X - 3, coord.Y }, console.io_int_to_hex(logs[position].result, 2), color);
		}
		else { 
			console.io_write_text(coord, debugLog->chain[position], WHITE_ON_BLACK);
		}
	}
}

void Window::Update(uint16_t tBottomIndex, uint16_t tCurrentSel) {
	COORD coord = coordTL;
	uint16_t max_count = (coordBR.Y - coordTL.Y) + 1;
	uint16_t display_count = min(tBottomIndex + 1, max_count);

	console.clear_rectangle(coordTL, coordBR);
	console.clear_rectangle(COORD_DEBUG_TL, COORD_DEBUG_BR);

	tBottomIndex = min(type == LOG ? logs.size() : debugLog->chain.size(), tBottomIndex);
	coord.Y = coordBR.Y;

	for (uint16_t i = 0; i < display_count; i++) {
		uint16_t position = tBottomIndex - i;
		uint16_t color;
		
		coord.Y = coordBR.Y - i;

		if (type == LOG) {
			if (logs[position].level == SUCCES) { color = GREEN_ON_BLACK; }
			else if (logs[position].level == AVERTISSEMENT) { color = YELLOW_ON_BLACK; }
			else if (logs[position].level == ERREUR) { color = RED_ON_BLACK; }
			else { color = WHITE_ON_BLACK; }
			if (i == tCurrentSel) { color = COLOR_INVERT(color); }

			console.io_write_text(coord, logs[position].strText, color);
			console.io_write_text({ coordBR.X - 3, coord.Y }, console.io_int_to_hex(logs[position].result, 2), color);
		}
		else {
			if (i == tCurrentSel) { console.io_write_text(coord, debugLog->chain[position], BLACK_ON_WHITE); }
			else { console.io_write_text(coord, debugLog->chain[position], WHITE_ON_BLACK); }
		}
	}

	console.cursor_set_color(WHITE_ON_BLACK);
}

void Window::Navigate(void) {
	uint16_t size = (type == LOG ? logs.size() : debugLog->chain.size());
	uint16_t windowOverFlow = coordBR.Y - coordTL.Y < size ? true : false;
	uint16_t lastIndex = size - 1;
	uint16_t bottomIndex = size - 1;
	uint16_t currentIndex = size - 1;
	uint16_t selectedLog = 0;
	uint16_t maxLogCount = min(coordBR.Y - coordTL.Y, size);

	if (size > 0) {
		Update(size - 1, 0);

		while (1) {
			uint8_t key = console.io_getch();
			currentIndex = bottomIndex - selectedLog;

			if (key == KEY_DOWN && selectedLog == 0 && bottomIndex != lastIndex) {	
				bottomIndex++;
				Update(bottomIndex, selectedLog);
			}
			else if (key == KEY_DOWN && selectedLog > 0) {									
				selectedLog--;
				Update(bottomIndex, selectedLog);
			}
			else if (key == KEY_UP && selectedLog == maxLogCount && windowOverFlow == true && currentIndex > 0) {		
				bottomIndex--;
				Update(bottomIndex, selectedLog);
			}
			else if (key == KEY_UP && selectedLog < maxLogCount && currentIndex > 0) {			
				selectedLog++;
				Update(bottomIndex, selectedLog);
			}
			else if (key == KEY_ENTER && type == LOG && logs[currentIndex].chain.size() > 0) {
				Window debugWindow({ coordTL.X, coordBR.Y + 3 }, { coordBR.X, coordBR.Y + 6 }, DEBUG);
				debugWindow.debugLog = &logs[currentIndex];
				debugWindow.Navigate();
				console.clear_window(&debugWindow);
			}
			else if (key == KEY_HELP && type == LOG) {
				console.io_write_text(COORD_DEBUG_LEVEL, ips.code_get_level_text(logs[currentIndex].result), WHITE_ON_BLACK);
				console.io_write_text(COORD_DEBUG_TEXT, ips.code_get_text(logs[currentIndex].result), WHITE_ON_BLACK);
			}
			else if (key == KEY_BACK || key == KEY_ESC) {										
				console.clear_rectangle(COORD_DEBUG_TL, COORD_DEBUG_BR);
				Update();
				return;
			}
		}
	}
}

void Window::Clear(void) {
	logs.clear();
	console.clear_rectangle(coordTL, coordBR);
}
