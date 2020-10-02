#pragma once
#include <iostream>
#include <windows.h>
using namespace std;


#define Ouvrir				Open
#define Fermer				Close
#define Lire				Read
#define Ecrire				Write
#define VerifierDonnees		CheckData


class SerialPort
{
public:

private:
	HANDLE hComDev;
	BOOL opened;
	uint16_t mPort;
	uint16_t mBaud;
	char szPort[15];
	char szComParams[50];

public:
	SerialPort(void);
	~SerialPort(void);
	BOOL Open(int port = 4, int baud = 9600);
	BOOL Close(void);
	int Read(void* tampon, int limite);
	int Write(const char* tampon, int dimension);
	int CheckData(void);
	const char* getNomPort(void);

private:

};