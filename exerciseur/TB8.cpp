#include <iomanip>
#include <algorithm>
#include <vector>
#include "TB8.h"
#include "Console.h"
#include "IPS.h"
#include "codes.h"
#include "Menu.h"
#include "Window.h"
using namespace std;


#define MAX_FRAME_COUNT			64


extern IPS ips;
extern Console console;


TB8::TB8(void) {
	currentMenuData_t = &MENU_MAIN;
}

TB8::~TB8(void) {

}

void TB8::empty(void) {

}

void TB8::loop(void) {
	Menu menu(currentMenuData_t);
	currentMenu = &menu;
	currentMenu->Navigate();
}


void TB8::BuildLog(string beg, uint8_t result, uint8_t* data, uint8_t count) {
	stringstream str;
	WindowLog log;
	uint8_t success = ips.code_get_level(result) <= AVERTISSEMENT;

	str << beg << "Data = ";

	if (count > 1) {
		str << "Chaine..";

		if (success) {
			while (count--) { log.chain.push_back(console.io_int_to_dec(count, 2) + " : " + console.io_int_to_hex(data[count], 2, true)); }
		}
	}
	else { 
		str << console.io_int_to_hex(data[0], 2, success); 
	}

	for (uint8_t i = str.str().length(); i < (currentMenu->currentWindow->coordBR.X - currentMenu->currentWindow->coordTL.X) - 3; i++) {
		str << " ";
	}

	log.strText = str.str();
	log.result = result;
	log.level = ips.code_get_level(result);

	currentMenu->currentWindow->Log(log);
}

/************************************************************************************/

void TB8::gpio_read(void) {
	stringstream text;
	uint8_t result;
	uint32_t address;
	uint32_t data = 0x00;

	console.io_write_text(COORD_INPUT_1, "Adresse : ", WHITE_ON_BLACK);
	if (console.io_input(&address, 2, BASE_HEX)) {

		result = ips.gpio_read(address, (uint8_t*)&data);

		text << "[ L ]  Add = " << console.io_int_to_hex(address, 2) << "  ";
		BuildLog(text.str(), result, (uint8_t*)&data, 1);
	}
}

void TB8::gpio_write(void) {
	stringstream text;
	uint8_t result;
	uint32_t address;
	uint32_t data;

	console.io_write_text(COORD_INPUT_1, "Adresse : ", WHITE_ON_BLACK);
	if (console.io_input(&address, 2, BASE_HEX)) {

		console.io_write_text(COORD_INPUT_2, "Octet : ", WHITE_ON_BLACK);
		if (console.io_input(&data, 2, BASE_HEX)) {

			result = ips.gpio_write(address, data);

			text << "[ E ]  Add = " << console.io_int_to_hex(address, 2) << "  ";
			BuildLog(text.str(), result, (uint8_t*)&data, 1);
		}
	}
}

void TB8::memory_read(void) {
	stringstream text;
	uint8_t result;
	uint32_t address;
	uint8_t buffer[256] = {};
	uint32_t count = 1;

	console.io_write_text(COORD_INPUT_1, "Adresse : ", WHITE_ON_BLACK);
	if (console.io_input(&address, 4, BASE_HEX)) {

		console.io_write_text(COORD_INPUT_2, "Nombre : ", WHITE_ON_BLACK);
		console.io_input(&count, 2, BASE_DEC);

		result = ips.memory_read(address, buffer, count);
		text << "[ L ]  Add = " << console.io_int_to_hex(address, 4) << "  ";

		BuildLog(text.str(), result, buffer, count);
	}
}

void TB8::memory_write(void) {
	stringstream text;
	uint8_t result;
	uint32_t address;
	uint8_t buffer[256] = {};							//Peut seulement écrire 64 à cause des pages dans le LC256
	uint8_t count = 0;

	console.io_write_text(COORD_INPUT_1, "Adresse : ", WHITE_ON_BLACK);
	if (console.io_input(&address, 4, BASE_HEX)) {

		console.io_write_text(COORD_INPUT_2, "Octet : ", WHITE_ON_BLACK);
		count = console.io_input(buffer, 64, COORD_INPUT_2);
	}
	if (count > 0) {
		result = ips.memory_write(address, buffer, count);
		text << "[ E ]  Add = " << console.io_int_to_hex(address, 4) << "  ";
		BuildLog(text.str(), result, buffer, count);
	}
}

void TB8::uart3_read(void) {
	stringstream text;
	uint8_t result;
	uint8_t buffer[256] = {};
	uint32_t count;

	console.io_write_text(COORD_INPUT_1, "Nombre : ", WHITE_ON_BLACK);
	if (console.io_input(&count, 2, BASE_DEC)) {
		result = ips.uart3_read(buffer, (uint16_t*)&count);
		count = min(count, 128);

		BuildLog("[ L ]  ", result, buffer, count);
	}
}

void TB8::uart3_write(void) {
	stringstream text;
	uint8_t result;
	uint8_t buffer[256] = {};
	uint8_t count = 0;

	console.io_write_text(COORD_INPUT_1, "Octet : ", WHITE_ON_BLACK);
	count = console.io_input(buffer, MAX_FRAME_COUNT - 2, COORD_INPUT_1);
	if (count > 0) {
		result = ips.uart3_write(buffer, count);
		BuildLog("[ E ]  ", result, buffer, count);
	}
}

void TB8::uart3_get_count(void) {
	stringstream text;
	uint8_t result;
	uint16_t count;
	WindowLog log;

	result = ips.uart3_get_count(&count);

	text << "[ L ]  Count = " << count;
	for (uint8_t i = text.str().length(); i < (currentMenu->currentWindow->coordBR.X - currentMenu->currentWindow->coordTL.X) - 3; i++) {
		text << " ";
	}

	log.strText = text.str();
	log.result = result;
	log.level = ips.code_get_level(result);
	currentMenu->currentWindow->Log(log);
}

void TB8::rs232_read(void) {
	stringstream text;
	uint8_t result;
	uint8_t buffer[64] = {};
	uint32_t count = 1;

	console.io_write_text(COORD_INPUT_1, "Nombre : ", WHITE_ON_BLACK);
	if (console.io_input(&count, 2, BASE_DEC)) {
		result = ips.rs232_read(buffer, (uint16_t*)&count);
		BuildLog("[ L ]  ", result, buffer, count);
	}
}

void TB8::rs232_write(void) {
	stringstream text;
	uint8_t result;
	uint8_t buffer[64] = {};
	uint8_t count = 0;

	console.io_write_text(COORD_INPUT_1, "Octet : ", WHITE_ON_BLACK);
	count = console.io_input(buffer, MAX_FRAME_COUNT - 2, COORD_INPUT_1);
	if (count > 0) {
		result = ips.rs232_write(buffer, count);
		BuildLog("[ E ]  ", result, buffer, count);
	}
}

void TB8::rs232_get_count(void) {
	stringstream text;
	uint8_t result;
	uint16_t count;
	WindowLog log;

	result = ips.rs232_get_count(&count);

	text << "[ L ]  Count = " << count;
	for (uint8_t i = text.str().length(); i < (currentMenu->currentWindow->coordBR.X - currentMenu->currentWindow->coordTL.X) - 3; i++) {
		text << " ";
	}

	log.strText = text.str();
	log.result = result;
	log.level = ips.code_get_level(result);
	currentMenu->currentWindow->Log(log);
}


void TB8::motorola_read(void) {
	stringstream text;
	uint8_t result;
	uint32_t address;
	uint32_t data = 0x00;

	console.io_write_text(COORD_INPUT_1, "Adresse : ", WHITE_ON_BLACK);
	if (console.io_input(&address, 2, BASE_HEX)) {
		result = ips.motorola_read(address, (uint8_t*)&data);
		text << "[ L ]  Add = " << console.io_int_to_hex(address, 2) << "  ";
		BuildLog(text.str(), result, (uint8_t*)&data, 1);
	}
}

void TB8::motorola_write(void) {
	stringstream text;
	uint8_t result;
	uint32_t address;
	uint32_t data;

	console.io_write_text(COORD_INPUT_1, "Adresse : ", WHITE_ON_BLACK);
	if (console.io_input(&address, 2, BASE_HEX)) {

		console.io_write_text(COORD_INPUT_2, "Octet : ", WHITE_ON_BLACK);
		if (console.io_input(&data, 2, BASE_HEX)) {

			result = ips.motorola_write(address, data);

			text << "[ E ]  Add = " << console.io_int_to_hex(address, 2) << "  ";
			BuildLog(text.str(), result, (uint8_t*)&data, 1);
		}
	}
}

void TB8::rs232_set_DTR(void) {
	WindowLog log;
	string options[3] = { "Baisser", "Monter" };
	uint8_t values[3] = { 0, 1 };
	uint8_t var;

	console.io_write_text(COORD_INPUT_1, "Modification : ", WHITE_ON_BLACK);

	if (console.io_input_cycle(options, values, &var, 0, 2)) {
		log.level = SUCCES;
		log.result = UART_OK;
		if (var == 0) {
			ips.rs232_disable_DTR();
			log.strText = "[ M ]  Ligne DTR baissee";
		}
		else if (var == 1) {
			ips.rs232_enable_DTR();
			log.strText = "[ M ]  Ligne DTR montee";
		}
		currentMenu->currentWindow->Log(log);
	}
}

void TB8::rs232_set_RTS(void) {
	WindowLog log;
	string options[3] = { "Baisser", "Monter" };
	uint8_t values[3] = { 0, 1 };
	uint8_t var;

	console.io_write_text(COORD_INPUT_1, "Modification : ", WHITE_ON_BLACK);

	if (console.io_input_cycle(options, values, &var, 0, 2)) {
		log.level = SUCCES;
		log.result = UART_OK;
		if (var == 0) {
			ips.rs232_disable_RTS();
			log.strText = "[ M ]  Ligne RTS baissee";
		}
		else if (var == 1) {
			ips.rs232_enable_RTS();
			log.strText = "[ M ]  Ligne RTS montee";
		}
		currentMenu->currentWindow->Log(log);
	}
}

void TB8::rs232_set_flow_control(void) {
	WindowLog log;
	string options[3] = { "Desactiver", "Activer" };
	uint8_t values[3] = { 0, 1 };
	uint8_t var;

	console.io_write_text(COORD_INPUT_1, "Modification : ", WHITE_ON_BLACK);

	if (console.io_input_cycle(options, values, &var, 0, 2)) {
		log.level = SUCCES;
		log.result = UART_OK;
		if (var == 0) {
			ips.rs232_disable_flow_control();
			log.strText = "[ M ]  Flow auto desactive";
		}
		else if (var == 1) {
			ips.rs232_enable_flow_control();
			log.strText = "[ M ]  Flow auto active";
		}
		currentMenu->currentWindow->Log(log);
	}
}

void TB8::rs232_mod_param(void) {
	WindowLog log;

	console.cursor_set_position(COORD_INPUT_1);
	if (ips.uart_set_param(IPS::RS232)) {
		log.level = SUCCES;
		log.result = UART_OK;
		log.strText = "[ M ]  Parametres modifies";
		currentMenu->currentWindow->Log(log);
	}
}

void TB8::uart_mod_param(void) {
	WindowLog log;

	console.cursor_set_position(COORD_INPUT_1);
	if (ips.uart_set_param(IPS::UART3)) {
		log.level = SUCCES;
		log.result = UART_OK;
		log.strText = "[ M ]  Parametres modifies";
		currentMenu->currentWindow->Log(log);

	}
}

/************************************************************************************/

void TB8::gpio_Debug(void) {
	uint8_t key;
	uint8_t byte;
	uint8_t result;
	uint16_t errorCount;
	COORD coord = { 28, 2 };
	uint8_t readAddress[4] = { 0x42, 0x40, 0x46, 0x44 };
	uint8_t writeAddress[4] = { 0x40, 0x42, 0x44, 0x46 };
	string text[4] = { "GPIO0 --> GPIO1", "GPIO1 --> GPIO0", "GPIO2 --> GPIO3", "GPIO3 --> GPIO2" };
	WindowLog log;

	console.io_write_text(coord, "1 - Placez un cable plat entre GPIO0 et GPIO1", WHITE_ON_BLACK);
	coord.Y += 1;
	console.io_write_text(coord, "2 - Placez un cable plat entre GPIO2 et GPIO3", WHITE_ON_BLACK);
	coord.Y += 1;
	console.io_write_text(coord, "3 - Configurez les adresses suivantes :", WHITE_ON_BLACK);
	coord.Y += 1;
	console.io_write_text(coord, "    GPIO0 = 0x40", WHITE_ON_BLACK);
	coord.Y += 1;
	console.io_write_text(coord, "    GPIO1 = 0x42", WHITE_ON_BLACK);
	coord.Y += 1;
	console.io_write_text(coord, "    GPIO2 = 0x44", WHITE_ON_BLACK);
	coord.Y += 1;
	console.io_write_text(coord, "    GPIO3 = 0x46", WHITE_ON_BLACK);
	coord.Y += 2;
	console.io_write_text(coord, "\"Entree\" pour commencer", WHITE_ON_BLACK);

	while (1) {
		key = console.io_getch();
		if (key == KEY_ENTER || key == KEY_ESC) {
			console.clear_rectangle({ 28,0 }, { 79,24 });
			if (key == KEY_ENTER) { break; }
			else if (key == KEY_ESC) { return; }
		}
	}

	coord = { 28, 2 };
	errorCount = 0;
	for (uint8_t i = 0; i < 4; i++) {
		console.io_write_text(coord, text[i] + " : ", WHITE_ON_BLACK);

		for (uint16_t j = 0; j < 256; j++) {
			byte = j + 1;

			result = ips.gpio_write(writeAddress[i], j);
			if (ips.code_get_level(result) > AVERTISSEMENT) {
				log.chain.push_back("[ E ]  " + text[i] + "   " + console.io_int_to_hex(j, 2) + "    " + console.io_int_to_hex(result, 2));
				errorCount++;
			}
			else {
				result = ips.gpio_read(readAddress[i], &byte);
				if (byte != j) {
					log.chain.push_back("[ L ]  " + text[i] + "   " + console.io_int_to_hex(byte, 2) + " != " + console.io_int_to_hex(j, 2));
					errorCount++;
				}
				else if (ips.code_get_level(result) > AVERTISSEMENT) {
					log.chain.push_back("[ L ]  " + text[i] + "   " + console.io_int_to_hex(j, 2) + "    " + console.io_int_to_hex(result, 2));
					errorCount++;
				}
			}

		}
		cout << int(256 - errorCount) << "/256";
		errorCount = 0;
		coord.Y += 1;
	}

	if (log.chain.size() > 0) {
		console.io_write_text({ 28, 7 }, "Liste des erreurs", RED_ON_BLACK);
		Window debugWindow({ 28, 9 }, { 64, 20 }, Window::DEBUG);
		debugWindow.debugLog = &log;
		debugWindow.Navigate();
		console.clear_window(&debugWindow);
	}
	else {
		console.io_write_text({ 28, 7 }, "Aucune erreur detectee", GREEN_ON_BLACK);
		console.io_getch();
	}

	console.clear_rectangle({ 28,0 }, { 79,24 });
}

void TB8::memory_Debug(void) {
	uint8_t key;
	uint8_t byte;
	uint8_t result;
	uint16_t errorCount;
	uint16_t count;
	uint16_t address;
	COORD coord = { 28, 2 };
	WindowLog log;

	struct data_t {
		uint16_t baseAddress;
		uint16_t pageFirstAddress;
		uint8_t length;
		BOOL writen;
		vector<uint8_t> data;
	};
	vector<data_t> values;
	vector<uint16_t> duplicates;

	console.io_write_text(coord, "1 - ATTENTION, ce test pourrait entrainer une", WHITE_ON_BLACK);
	coord.Y += 1;
	console.io_write_text(coord, "    perte de donnees dans la memoire EEPROM", WHITE_ON_BLACK);
	coord.Y += 2;
	console.io_write_text(coord, "\"Entree\" pour commencer", WHITE_ON_BLACK);

	while (1) {
		key = console.io_getch();
		if (key == KEY_ENTER || key == KEY_ESC) {
			console.clear_rectangle({ 28,0 }, { 79,24 });
			if (key == KEY_ENTER) { break; }
			else if (key == KEY_ESC) { return; }
		}
	}

	while (1) {
		coord = { 28, 1 };
		values.clear();
		duplicates.clear();
		srand(time(NULL));

		coord.Y += 1;
		console.io_write_text(coord, "Generation de valeurs : ", WHITE_ON_BLACK);
		for (uint16_t i = 0; i < 10; i++) {
			values.push_back(data_t());

			values[i].baseAddress = rand() % 0xFFFF;
			values[i].length = rand() % 64 + 1;
			values[i].pageFirstAddress = values[i].baseAddress - (values[i].baseAddress % 64);

			for (uint8_t j = 0; j < values[i].length; j++) {
				values[i].data.push_back(rand() % 0xFF);
			}

			for (uint16_t j = 0; j < values[i].length; j++) {
				address = values[i].pageFirstAddress + ((values[i].baseAddress + j) % 64);
				duplicates.push_back(address);
			}
		}
		cout << "OK";

		coord.Y += 1;
		console.io_write_text(coord, "Test de doublons : ", WHITE_ON_BLACK);
		sort(duplicates.begin(), duplicates.end());
		auto iterator = unique(duplicates.begin(), duplicates.end());
		if (iterator == duplicates.end()) {
			cout << "OK";
			break;
		}
	}

	coord.Y += 1;
	errorCount = 0;
	console.io_write_text(coord, "Ecriture des donnees : ", WHITE_ON_BLACK);
	for (uint16_t i = 0; i < values.size(); i++) {
		result = ips.memory_write(values[i].baseAddress, &values[i].data[0], values[i].length);

		if (ips.code_get_level(result) > AVERTISSEMENT) {
			log.chain.push_back("[ E ]  Add = " + console.io_int_to_hex(values[i].baseAddress, 4) + "            " + console.io_int_to_hex(result, 2));
			errorCount++;
			values[i].writen = false;
		}
		else {
			values[i].writen = true;
		}

		Sleep(10);
	}
	cout << int(10 - errorCount) << "/" << int(10);

	coord.Y += 1;
	errorCount = 0;
	console.io_write_text(coord, "Lecture des donnees : ", WHITE_ON_BLACK);
	for (uint16_t i = 0; i < values.size(); i++) {
		if (values[i].writen == true) {
			for (uint16_t j = 0; j < values[i].length; j++) {
				address = values[i].pageFirstAddress + ((values[i].baseAddress + j) % 64);
				result = ips.memory_read(address, &byte, 1);

				if (byte != values[i].data[j]) {
					log.chain.push_back("[ L ]  Add = " + console.io_int_to_hex(address, 4) + "    " + console.io_int_to_hex(byte, 2) + " != " + console.io_int_to_hex(values[i].data[j], 2));
					errorCount++;
				}
				else if (ips.code_get_level(result) > AVERTISSEMENT) {
					log.chain.push_back("[ L ]  Add = " + console.io_int_to_hex(address, 4) + "            " + console.io_int_to_hex(result, 2));
					errorCount++;
				}
			}
		}
		else {
			errorCount += values[i].data.size();
		}
	}
	cout << int(duplicates.size() - errorCount) << "/" << int(duplicates.size());

	if (log.chain.size() > 0) {
		console.io_write_text({ 28, 7 }, "Liste des erreurs", RED_ON_BLACK);
		Window debugWindow({ 28, 9 }, { 62, 20 }, Window::DEBUG);
		debugWindow.debugLog = &log;
		debugWindow.Navigate();
		console.clear_window(&debugWindow);
	}
	else {
		console.io_write_text({ 28, 7 }, "Aucune erreur detectee", GREEN_ON_BLACK);
		console.io_getch();
	}

	console.clear_rectangle({ 28, 0 }, { 79, 24 });
}

void TB8::uart3_Debug(void) {
	uint8_t key;
	uint8_t result;
	//uint16_t errorCount = 0;
	uint16_t count = 1;
	uint16_t invalid = 0;
	COORD coord = { 28, 2 };
	WindowLog log;

	uint8_t bufferRX[2048];
	uint8_t bufferTX[4096];

	console.io_write_text(coord, "1 - Placez un cavalier sur le header TX et RX", WHITE_ON_BLACK);
	coord.Y += 2;
	console.io_write_text(coord, "\"Entree\" pour commencer", WHITE_ON_BLACK);

	while (1) {
		key = console.io_getch();
		if (key == KEY_ENTER || key == KEY_ESC) {
			console.clear_rectangle({ 28,0 }, { 79,24 });
			if (key == KEY_ENTER) { break; }
			else if (key == KEY_ESC) { return; }
		}
	}

	result = ips.uart3_read(bufferRX, count);									//Effacer les erreurs

	coord = { 28, 2 };
	srand(time(NULL));
	console.io_write_text(coord, "Generation de valeurs : ", WHITE_ON_BLACK);
	for (uint16_t i = 0; i < 4096; i++) { bufferTX[i] = rand() % 0xFF; }
	cout << "OK";

	coord.Y += 1;
	console.io_write_text(coord, "Reinitialisation du FIFO : ", WHITE_ON_BLACK);
	result = ips.uart3_get_count(&count);
	if (result == UART_OK) {
		result = ips.uart3_read(bufferRX, count);
		if (result == UART_OK) { cout << "OK"; }
		else { cout << "ERREUR"; }
	}
	else { cout << "ERREUR"; }

	coord.Y += 1;
	count = 0;
	console.io_write_text(coord, "Test d'ecriture : ", WHITE_ON_BLACK);
	for (uint16_t i = 1; i < 2048; i <<= 1) {
		result = ips.uart3_write(&bufferTX[i], i);

		if (ips.code_get_level(result) > AVERTISSEMENT) {
			log.chain.push_back("[ E ]  Qte = " + console.io_int_to_dec(i, 4) + "              " + console.io_int_to_hex(result, 2));
			invalid |= i;
			count += i;
		}
	}
	cout << int(2047 - count) << "/" << int(2047);

	coord.Y += 1;
	count = 0;
	console.io_write_text(coord, "Test de lecture : ", WHITE_ON_BLACK);
	for (uint16_t i = 1; i < 2048; i <<= 1) {
		if ((i & invalid) == false) {
			result = ips.uart3_read(&bufferRX[i], i);

			if (ips.code_get_level(result) > AVERTISSEMENT) {
				log.chain.push_back("[ L ]  Qte = " + console.io_int_to_dec(i, 4) + "              " + console.io_int_to_hex(result, 2));
				count += i;
			}
			else {
				for (uint16_t j = i; j < (i << 1); j++) {
					if (bufferRX[j] != bufferTX[j]) {
						log.chain.push_back("[ L ]  Num = " + console.io_int_to_dec(j, 4) + "     " + console.io_int_to_hex(bufferRX[j], 2) + " != " + console.io_int_to_hex(bufferTX[j], 2));
						count++;
					}
				}
			}
		}
		else {
			count += i;
		}

	}
	cout << int(2047 - count) << "/" << int(2047);

	coord.Y += 1;
	console.io_write_text(coord, "Test du FIFO : ", WHITE_ON_BLACK);
	result = ips.uart3_write(&bufferTX[2048], 2048);
	result = ips.uart3_read(bufferRX, 2048);
	if (result != UART_OK) {
		cout << "ERREUR";
		log.chain.push_back("Test de FIFO : ERREUR              ");

		coord.Y += 1;
		console.io_write_text(coord, "Test de debordement : NON EFFECTUE", WHITE_ON_BLACK);
	}
	else {
		cout << "OK";

		coord.Y += 1;
		console.io_write_text(coord, "Test de debordement : ", WHITE_ON_BLACK);	
		count = 1;
		result = ips.uart3_write(bufferTX, 2048);
		result = ips.uart3_write(bufferTX, 1);
		result = ips.uart3_read(bufferRX, &count);
		if (result == UART_OVERRUN) {
			cout << "OK";
		}
		else {
			cout << "ERREUR";
			log.chain.push_back("Test de debordement : ERREUR       ");
		}
	}

	if (log.chain.size() > 0) {
		console.io_write_text({ 28, 9 }, "Liste des erreurs", RED_ON_BLACK);
		Window debugWindow({ 28, 11 }, { 62, 20 }, Window::DEBUG);
		debugWindow.debugLog = &log;
		debugWindow.Navigate();
		console.clear_window(&debugWindow);
	}
	else {
		console.io_write_text({ 28, 9 }, "Aucune erreur detectee", GREEN_ON_BLACK);
		console.io_getch();
	}

	console.clear_rectangle({ 28, 0 }, { 79, 24 });
}

void TB8::rs232_Debug(void) {

}

void TB8::motorola_Debug(void) {

}