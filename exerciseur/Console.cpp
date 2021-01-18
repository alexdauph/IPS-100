#include <iomanip>
#include <sstream>
#include <conio.h>
#include "Console.h"
#include "Window.h"
#include "IPS.h"

/************************************************************************************/
/* CETTE PARTIE DE CODE EST DEJA INTEGREE DANS LA LIBRAIRIE IPS.lib */
/* NE PAS ENLEVER #if false SINON PROBLEME DE COMPATIBILITE*/
/************************************************************************************/

void Console::draw_window(Window* window) {
	COORD coord1;
	COORD coord2;

	if (window->coordTL.X == window->coordBR.X && window->coordTL.Y == window->coordBR.Y) { return; }

	cursor_set_color(WHITE_ON_BLACK);

	coord1.X = window->coordTL.X - 1;
	coord1.Y = window->coordTL.Y - 1;
	coord2.X = window->coordBR.X + 1;
	coord2.Y = window->coordBR.Y + 1;

	draw_rectangle(coord1, coord2);
}

void Console::clear_window(Window* window) {
	clear_rectangle({ window->coordTL.X - 1,window->coordTL.Y - 1 }, { window->coordBR.X + 1,window->coordBR.Y + 1 });
}


#if false

extern IPS ips;


Console::Console() {
	system("CLS");
}

/************************************************************************************/

void Console::draw_line(COORD coordTL, COORD coordBR) {
	COORD coord = coordTL;

	cursor_set_position(coord);

	if (coordTL.Y == coordBR.Y) {		//Horizontal
		for (uint8_t i = coordTL.X; i < coordBR.X; i++) {
			cout << char(196);
		}
	}
	else {								//Vertical
		for (uint8_t i = coordTL.Y; i <= coordTL.Y + (coordBR.Y - coordTL.Y); i++) {
			coord.Y = i;
			cursor_set_position(coord);
			cout << char(179);
		}
	}
}

void Console::draw_separation(COORD coordTL, COORD coordBR) {
	////COORD coord = coordTL;

	////cursor_set_position(coord);
	//draw_line(coordTL, coordBR);

	//if (coordTL.Y == coordBR.Y) {		//Horizontal
	//	cursor_set_position({ coordTL.X,coordTL.Y - 1 });
	//	cout << char(194);
	//	cursor_set_position({ coordBR.X,coordBR.Y + 1 });
	//	cout << char(193);
	//}
	//else {								//Vertical
	//	cursor_set_position({ coordTL.X,coordTL.Y - 1 });
	//	cout << char(194);
	//	cursor_set_position({ coordBR.X,coordBR.Y + 1 });
	//	cout << char(193);
	//}
}

void Console::draw_corners(COORD coordTL, COORD coordBR) {
	COORD coord = coordTL;

	cursor_set_position(coord);
	cout << char(218);				//Top left	

	coord.X = coordBR.X;
	cursor_set_position(coord);
	cout << char(191);				//Top right

	coord.Y = coordBR.Y;
	cursor_set_position(coord);
	cout << char(217);				//Bottom left

	coord.X = coordTL.X;
	cursor_set_position(coord);
	cout << char(192);				//Bottom right
}

void Console::draw_rectangle(COORD coordTL, COORD coordBR) {
	COORD coord1;
	COORD coord2;

	coord1.X = coordTL.X;
	coord1.Y = coordTL.Y;
	coord2.X = coordTL.X;
	coord2.Y = coordBR.Y;
	draw_line(coord1, coord2);		//Left

	coord1.X = coordBR.X;
	coord1.Y = coordTL.Y;
	coord2.X = coordBR.X;
	coord2.Y = coordBR.Y;
	draw_line(coord1, coord2);		//Right

	coord1.X = coordTL.X;
	coord1.Y = coordTL.Y;
	coord2.X = coordBR.X;
	coord2.Y = coordTL.Y;
	draw_line(coord1, coord2);		//Top

	coord1.X = coordTL.X;
	coord1.Y = coordBR.Y;
	coord2.X = coordBR.X;
	coord2.Y = coordBR.Y;
	draw_line(coord1, coord2);		//Bottom

	draw_corners(coordTL, coordBR);
}

void Console::clear_rectangle(COORD coordTL, COORD coordBR) {
	COORD coord = coordTL;
	string str;

	cursor_set_color(WHITE_ON_BLACK);

	for (uint8_t i = 0; i <= (coordBR.X - coordTL.X); i++) {
		str += " ";
	}
	for (uint8_t i = coordTL.Y; i <= coordBR.Y; i++) {
		coord.Y = i;
		cursor_set_position(coord);
		cout << str;
	}
}

void Console::draw_window(Window* window) {
	COORD coord1;
	COORD coord2;

	if (window->coordTL.X == window->coordBR.X && window->coordTL.Y == window->coordBR.Y) { return; }

	cursor_set_color(WHITE_ON_BLACK);

	coord1.X = window->coordTL.X - 1;
	coord1.Y = window->coordTL.Y - 1;
	coord2.X = window->coordBR.X + 1;
	coord2.Y = window->coordBR.Y + 1;

	draw_rectangle(coord1, coord2);
}

void Console::clear_window(Window* window) {
	clear_rectangle({ window->coordTL.X - 1,window->coordTL.Y - 1 }, { window->coordBR.X + 1,window->coordBR.Y + 1 });
}

/************************************************************************************/

void Console::screen_save(void) {
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &mScreenInfo);
	mScreenCursorPos = cursor_get_position();

	ReadConsoleOutputA(
		GetStdHandle(STD_OUTPUT_HANDLE),							// screen buffer to read from 
		mScreenData,												// buffer to copy into 
		{ 80, 25 },													// col-row size of chiBuffer 
		{ mScreenInfo.srWindow.Top, mScreenInfo.srWindow.Left },	// top left dest. cell in chiBuffer 
		&mScreenInfo.srWindow);										// screen buffer source rectangle 
}

void Console::screen_print(void) {
	cursor_set_position({ mScreenInfo.srWindow.Top, mScreenInfo.srWindow.Left });
	cursor_set_position(mScreenCursorPos);

	WriteConsoleOutputA(
		GetStdHandle(STD_OUTPUT_HANDLE),							// screen buffer to write to
		mScreenData,												// buffer to copy from
		{ 80, 25 },													// col-row size of chiBuffer 
		{ mScreenInfo.srWindow.Top, mScreenInfo.srWindow.Left },	// top left dest. cell in chiBuffer 
		&mScreenInfo.srWindow);
}

void Console::screen_clear(void) {
	CONSOLE_SCREEN_BUFFER_INFO screenInfo;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &screenInfo);
	clear_rectangle({ screenInfo.srWindow.Left, screenInfo.srWindow.Top }, { screenInfo.srWindow.Right, screenInfo.srWindow.Bottom });
}

uint16_t Console::screen_get_top(void) {
	CONSOLE_SCREEN_BUFFER_INFO screenInfo;

	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &screenInfo);
	return screenInfo.srWindow.Top;
}

string Console::io_int_to_dec(uint32_t value, uint8_t tCount) {
	stringstream str;
	str << setfill('0') << setw(tCount) << right << uppercase << int(value);
	return str.str();
}

string Console::io_int_to_hex(uint32_t value, uint8_t tCount) {
	stringstream str;
	str << "0x" << setfill('0') << setw(tCount) << right << uppercase << hex << int(value);
	return str.str();
}

string Console::io_int_to_hex(uint32_t value, uint8_t tCount, BOOL isValid) {
	stringstream str;

	if (isValid) {
		str << io_int_to_hex(value, tCount);
		if (IS_CARACTER(value)) { str << " '" << char(value) << "'"; }
		else { str << "    "; }
	}
	else {
		str << "        ";
	}

	return str.str();
}

void Console::io_write_text(uint8_t posX, uint8_t posY, string text, uint8_t color) {
	COORD coord;
	coord.X = posX;
	coord.Y = posY;
	io_write_text(coord, text, color);
}

void Console::io_write_text(COORD cursorCoord, string text, uint8_t color) {
	cursor_set_position(cursorCoord);
	cursor_set_color(color);
	cout << text;
	cursor_set_color(WHITE_ON_BLACK);
}

void Console::io_write_text(string text, uint8_t color) {
	cursor_set_color(color);
	cout << text;
	cursor_set_color(WHITE_ON_BLACK);
}

/************************************************************************************/

void Console::cursor_set_visibility(uint8_t status) {
	CONSOLE_CURSOR_INFO CURSOR;
	CURSOR.dwSize = 1;
	CURSOR.bVisible = status;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CURSOR);
}

void Console::cursor_set_position(COORD cursorCoord) {
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorCoord);
}

COORD Console::cursor_get_position(void) {
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
	return info.dwCursorPosition;
}

void Console::cursor_set_line_beginning(COORD coord) {
	mLineBeginning = coord;
	cursor_set_position(coord);
}

void Console::cursor_change_line(uint8_t count) {
	mLineBeginning.Y += count;
	cursor_set_position(mLineBeginning);
}

void Console::cursor_set_color(uint8_t color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

/************************************************************************************/



/************************************************************************************/

uint8_t Console::io_getch(void) {
	uint8_t key = _getch();
	if (key == 224) {
		key = _getch();
		if (key == 0x48) { key = 0xC0; }
		else if (key == 0x50) { key = 0xC1; }
	}
	return key;
}

uint8_t Console::io_input(uint32_t* var, uint8_t tCount, uint8_t base) {
	string hexValues = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f','A','B','C','D','E','F' };
	string validValues;
	uint8_t key;
	uint8_t index = 0;
	uint8_t buffer[8];

	if (base == 10) { validValues = hexValues.substr(0, 10); }
	else if (base == 16) { validValues = hexValues; }

	cursor_set_visibility(true);

	while (1) {
		key = io_getch();

		if ((key == KEY_ENTER && index == 0) || key == KEY_ESC) {
			index = 0;
			break;
		}
		else if (key == KEY_ENTER && index > 0) {
			*var = strtol((char*)buffer, NULL, base);
			break;
		}
		else if (key == KEY_BACK && index > 0) {
			buffer[index - 1] = 0x00;
			index--;
			cout << "\b \b";
		}
		else if (validValues.find(key) != string::npos && index < tCount) {
			buffer[index] = key;
			index++;
			cout << char(key);
		}
	}

	cursor_set_visibility(false);
	return index;
}

uint8_t Console::io_input(uint8_t* buffer, uint8_t stringCount, COORD cursorCoord) {
	string hexValues = { '0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f','A','B','C','D','E','F' };
	uint8_t key;
	uint8_t count = 0;
	uint8_t mode = 0;
	BOOL changed = false;

	cursor_set_visibility(true);

	while (1) {
		key = io_getch();

		if ((key == KEY_ENTER && count == 0) || key == KEY_ESC) {
			count = 0;
			break;
		}
		else if (key == KEY_ENTER) {
			if (mode == 0) {
				buffer[0] = strtol((char*)buffer, NULL, 16);
				count = 1;
			}
			else if (mode == 1) {
				count = 1;
			}
			else if (mode == 2) {
				//Rien
			}
			break;
		}
		else if (key == KEY_BACK && count > 0) {
			buffer[count - 1] = 0x00;
			count--;
			cout << "\b \b";
		}
		else if (key == KEY_UP || key == KEY_DOWN) {
			if (key == KEY_UP && mode < 2) { 
				mode++; 
				changed = true;
			}
			else if (key == KEY_DOWN && mode > 0) { 
				mode--;
				changed = true;
			}

			if (changed == true) {
				clear_rectangle({ 0, cursorCoord.Y }, { 79, cursorCoord.Y });
				cursor_set_position(cursorCoord);
				count = 0;

				if (mode == 0) { cout << "Octet : "; }
				else if (mode == 1) { cout << "Caractere : "; }
				else if (mode == 2) { cout << "Chaine : "; }

				changed = false;
			}
		}
		else if (IS_CARACTER(key)) {
			if (mode == 0 && hexValues.find(key) != string::npos && count < 2) {
				buffer[count] = key;
				count++;
				cout << char(key);
			}
			else if (mode == 1 && count < 1) {
				buffer[count] = key;
				count++;
				cout << char(key);
			}
			else if (mode == 2 && count < stringCount) {
				buffer[count] = key;
				count++;
				cout << char(key);
			}
		}
	}

	cursor_set_visibility(false);
	return count;
}

uint8_t Console::io_input_cycle(const string* options, const uint8_t* tValues, uint8_t* var, uint8_t currentValue, uint8_t tCount) {
	uint8_t currentOption = 0;
	uint8_t key;

	for (uint8_t i = 0; i < tCount; i++) {
		if (tValues[i] == currentValue) {
			currentOption = i;
			break;
		}
	}

	cout << options[currentOption];

	while (1) {
		key = io_getch();

		if (key == KEY_UP && currentOption < tCount - 1) {
			for (uint8_t i = 0; i < options[currentOption].length(); i++) { cout << "\b \b"; }
			currentOption++;
			cout << options[currentOption];
		}
		else if (key == KEY_DOWN && currentOption > 0) {
			for (uint8_t i = 0; i < options[currentOption].length(); i++) { cout << "\b \b"; }
			currentOption--;
			cout << options[currentOption];
		}
		else if (key == KEY_ENTER) {
			*var = tValues[currentOption];
			return 1;
		}
		else if (key == KEY_BACK || key == KEY_ESC) {
			return 0;
		}
	}
}

#endif