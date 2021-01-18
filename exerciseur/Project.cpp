#include <iostream>
#include <windows.h>
#include "conio.h"
#include "IPS.h"
#include "TB8.h"
#include "Console.h"
#include "codes.h"


IPS ips;
TB8 tb8;

#define RUN_MODE 1

int main() {

	while (1) { tb8.loop(); }

	return 0;
}

