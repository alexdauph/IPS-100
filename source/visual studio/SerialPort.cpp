#include "SerialPort.h"


SerialPort::SerialPort() {
	hComDev = NULL;
	opened = FALSE;
}

SerialPort::~SerialPort() {
	Close();
}

BOOL SerialPort::Open(int port, int baud) {
	mPort = port;
	mBaud = baud;

	if (opened) return(TRUE);

	DCB dcb;
	BOOL fsuccess;

	wsprintf(szPort, "\\\\.\\COM%d", port);
	hComDev = CreateFile(szPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hComDev == INVALID_HANDLE_VALUE)
		return(FALSE);

	COMMTIMEOUTS CommTimeOuts;
	CommTimeOuts.ReadIntervalTimeout = 50;
	CommTimeOuts.ReadTotalTimeoutMultiplier = 10;
	CommTimeOuts.ReadTotalTimeoutConstant = 50;
	CommTimeOuts.WriteTotalTimeoutMultiplier = 10;
	CommTimeOuts.WriteTotalTimeoutConstant = 50;
	fsuccess = SetCommTimeouts(hComDev, &CommTimeOuts);

	if (!fsuccess) {
		//printf("Function SetCommTimeouts failed with error %d.\n", (int)GetLastError());
		return(FALSE);
	}

	wsprintf(szComParams, "\\\\.\\COM%d:%d,n,8,1", port, baud);

	memset(&dcb, 0, sizeof(DCB));
	dcb.DCBlength = sizeof(DCB);

	fsuccess = GetCommState(hComDev, &dcb);

	if (!fsuccess) {
		//printf("Function GetCommState failed with error %d.\n", (int)GetLastError());
		return(FALSE);
	}
	dcb.BaudRate = baud;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;

	fsuccess = SetCommState(hComDev, &dcb);
	if (!fsuccess) {
		DWORD dwError = GetLastError();
		//printf("Function SetCommState failed with error %d.\n", (int)dwError);
		CloseHandle(hComDev);
		return(FALSE);
	}

	opened = TRUE;

	return(opened);
}

BOOL SerialPort::Close(void) {
	if (!opened || hComDev == NULL)
		return(TRUE);

	CloseHandle(hComDev);
	opened = FALSE;
	hComDev = NULL;

	return(TRUE);
}

int SerialPort::Read(void* buffer, int limit) {
	BOOL etat;
	DWORD dwnCaracteresLus;

	if (!opened || hComDev == NULL)
		return(0);

	etat = ReadFile(hComDev, buffer, limit, &dwnCaracteresLus, NULL);
	if (!etat)
		return (0);

	return dwnCaracteresLus;
}

int SerialPort::Write(const char* tampon, int nCaracteres) {
	if (!opened || hComDev == NULL)
		return(0);

	DWORD dwDonneesEcrites = 0;
	BOOL etat;

	etat = WriteFile(hComDev,			// Handle du mPort série
		tampon,							// Données à envoyer sur le mPort
		nCaracteres,					// Nombre de caractères à écrire
		&dwDonneesEcrites,				//Nombre d'octets écrits
		NULL);

	if (!etat)
		return (0);

	return((int)dwDonneesEcrites);
}

BOOL SerialPort::CheckData(void) {

	if (!opened || hComDev == NULL)
		return (0);

	DWORD dwErrorFlags;
	COMSTAT ComStat;

	ClearCommError(hComDev, &dwErrorFlags, &ComStat);

	return ((int)ComStat.cbInQue);
}

const char* SerialPort::getNomPort(void) {
	return szPort;
}