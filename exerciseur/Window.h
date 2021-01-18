#pragma once
#include <iostream>
#include <vector>
#include <Windows.h>
using namespace std;


#define WINDOW_STD			Window::windows[0]
#define WINDOW_NONE			Window::windows[1]


class TB8;
enum LogType;
struct WindowData_t;


extern const COORD COORD_TITLE;
extern const COORD COORD_DEBUG_LEVEL;
extern const COORD COORD_DEBUG_TEXT;
extern const COORD COORD_DEBUG_TL;
extern const COORD COORD_DEBUG_BR;
extern const COORD COORD_OPTIONS;
extern const COORD COORD_INPUT_1;
extern const COORD COORD_INPUT_2;
extern const COORD COORD_INPUT_TL;
extern const COORD COORD_INPUT_BR;


struct WindowLog {
	vector<string> chain;
	string strText;
	uint8_t result;
	uint8_t level;
};

struct WindowData_t {
	COORD coordTL;
	COORD coordBR;
};


class Window {
private:

public:
	const enum WindowType {
		DEBUG,
		LOG,
	} type;

	const COORD coordTL;
	const COORD coordBR;
	WindowLog* debugLog;
	static WindowData_t windows[2];

private:
	vector<WindowLog> logs;

public:
	Window(COORD coordTL, COORD coordBR, WindowType tType);
	~Window();
	void empty(void);

	void Log(WindowLog tLog);
	void Update(void);
	void Update(uint16_t tBottomIndex, uint16_t tCurrentSel);
	void Navigate(void);
	void Clear(void);

	
};