#pragma once
#include <iostream>
#include <Windows.h>
using namespace std;


#define GREY_ON_BLACK			0x08
#define WHITE_ON_BLACK			0x0F
#define BLACK_ON_GREY			0x80
#define BLACK_ON_WHITE			0xF0
#define YELLOW_ON_BLACK			0x0E
#define GREEN_ON_BLACK			0x0A
#define RED_ON_BLACK			0x0C
#define BLACK_ON_YELLOW			0x60
#define BLACK_ON_GREEN			0xA0
#define BLACK_ON_RED			0xC0
#define MASK_BITCOUNT			0x03
#define MASK_STOPBITS			0x04
#define MASK_PARITY				0x38
#define KEY_UP					0xC0
#define KEY_DOWN				0xC1
#define KEY_BACK				0x08
#define KEY_ENTER				0x0D
#define KEY_TAB					0x09
#define KEY_ESC					0x1B
#define KEY_HELP				0x3F

#define COLOR_INVERT(color)		((color & 0x0F) << 4) | ((color & 0xF0) >> 4)
#define IS_CARACTER(val)		val > 0x1F && val < 0x7F


class Window;


class Console {
private:

	CHAR_INFO mScreenData[2000];					//[25][80]; 
	CONSOLE_SCREEN_BUFFER_INFO mScreenInfo;
	COORD mScreenCursorPos;
	COORD mLineBeginning;

public:

private:

public:
	Console();

	void draw_line(COORD coordTL, COORD coordBR);
	void draw_separation(COORD coordTL, COORD coordBR);
	void draw_corners(COORD coordTL, COORD coordBR);
	void draw_rectangle(COORD coordTL, COORD coordBR);
	void draw_window(Window* window);
	void clear_rectangle(COORD coordTL, COORD coordBR);
	void clear_window(Window* window);

	void screen_save(void);
	void screen_print(void);
	void screen_clear(void);
	uint16_t screen_get_top(void);

	string io_int_to_dec(uint32_t value, uint8_t count);
	string io_int_to_hex(uint32_t value, uint8_t count);
	string io_int_to_hex(uint32_t value, uint8_t count, BOOL isValid);
	void io_write_text(uint8_t posX, uint8_t posY, string text, uint8_t color);
	void io_write_text(COORD cursorCoord, string text, uint8_t color);
	void io_write_text(string text, uint8_t color);
	uint8_t io_getch(void);
	uint8_t io_input(uint32_t* var, uint8_t count, uint8_t base);
	uint8_t io_input(uint8_t* buffer, uint8_t stringCount, COORD cursorCoord);
	uint8_t io_input_cycle(const string* options, const uint8_t* tValues, uint8_t* var, uint8_t currentValue, uint8_t count);

	void cursor_set_position(COORD cursorCoord);
	COORD cursor_get_position(void);
	void cursor_set_line_beginning(COORD coord);
	void cursor_change_line(uint8_t count);
	void cursor_set_visibility(uint8_t status);
	void cursor_set_color(uint8_t color);

};