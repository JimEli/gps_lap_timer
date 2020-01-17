#include "serial_port.h"

PORT OpenPort(unsigned portNumber)
{
	HANDLE hComm;
	TCHAR comname[100];
	wsprintf(comname, TEXT("\\\\.\\COM%d"), portNumber);

	// Port name, read/write, no sharing, no security, open existing port only, non-overlapped I/O, Null for Comm devices.
	hComm = CreateFile(comname, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

	if (hComm == INVALID_HANDLE_VALUE)
		return NULL;

	COMMTIMEOUTS timeouts = { 0 };
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;

	if (SetCommTimeouts(hComm, &timeouts) == FALSE)
		return NULL;

	if (SetCommMask(hComm, EV_RXCHAR) == FALSE)
		return NULL;

	return hComm;
}

void ClosePort(PORT comPort) { CloseHandle(comPort); }

int SetBaudRate(PORT comPort, int rate)
{
	DCB dcbSerialParams = { 0 };
	BOOL status;

	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	status = GetCommState(comPort, &dcbSerialParams);
	if (status == FALSE)
		return FALSE;
	
	dcbSerialParams.BaudRate = rate;
	status = SetCommState(comPort, &dcbSerialParams);
	return status;
}

int SetDataBits(PORT comPort, int bits)
{
	DCB dcbSerialParams = { 0 };
	BOOL status;
	
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	status = GetCommState(comPort, &dcbSerialParams);
	if (status == FALSE)
		return FALSE;
	
	dcbSerialParams.ByteSize = bits;
	status = SetCommState(comPort, &dcbSerialParams);
	return status;
}

int SetStopBits(PORT comPort, int bits)
{
	DCB dcbSerialParams = { 0 };
	BOOL status;
	
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	status = GetCommState(comPort, &dcbSerialParams);
	if (status == FALSE)
		return FALSE;
	
	dcbSerialParams.StopBits = bits;
	status = SetCommState(comPort, &dcbSerialParams);
	return status;
}

int SetParity(PORT comPort, int parity)
{
	DCB dcbSerialParams = { 0 };
	BOOL status;

	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	status = GetCommState(comPort, &dcbSerialParams);
	if (status == FALSE)
		return FALSE;
	
	dcbSerialParams.Parity = parity;
	status = SetCommState(comPort, &dcbSerialParams);
	return status;
}

int GetBaudRate(PORT comPort)
{
	DCB dcbSerialParams = { 0 };
	BOOL status;
	
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	status = GetCommState(comPort, &dcbSerialParams);
	if (status == FALSE)
		return -1;
	
	return dcbSerialParams.BaudRate;
}

int GetDataBits(PORT comPort) 
{
	DCB dcbSerialParams = { 0 };
	BOOL status;

	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	status = GetCommState(comPort, &dcbSerialParams);
	if (status == FALSE)
		return -1;
	
	return dcbSerialParams.ByteSize;
}

int GetStopBits(PORT comPort) 
{
	DCB dcbSerialParams = { 0 };
	BOOL status;

	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	status = GetCommState(comPort, &dcbSerialParams);
	if (status == FALSE)
		return -1;
	
	return dcbSerialParams.StopBits;
}

int GetParity(PORT comPort) 
{
	DCB dcbSerialParams = { 0 };
	BOOL status;

	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	status = GetCommState(comPort, &dcbSerialParams);
	if (status == FALSE)
		return -1;

	return dcbSerialParams.Parity;
}

int SendData(PORT comPort, unsigned char* data, size_t size)
{
	//uint32_t  dNoOFBytestoWrite = strlen(data);
	uint32_t  dNoOFBytestoWrite = size;
	uint32_t  dNoOfBytesWritten;
	
	BOOL status = WriteFile(comPort, data, dNoOFBytestoWrite, &dNoOfBytesWritten, NULL);
	if (status == FALSE)
		return -1;

	return dNoOfBytesWritten;
}

int ReceiveData(PORT comPort, char* data, int len)
{
	uint32_t dwEventMask;
	uint32_t NoBytesRead;

	BOOL status = WaitCommEvent(comPort, &dwEventMask, NULL);
	if (status == FALSE) 
		return FALSE;
	
	status = ReadFile(comPort, data, len, &NoBytesRead, NULL);
	data[NoBytesRead] = 0;
	if (status == FALSE) 
		return FALSE;
	
	return TRUE;
}

int FlushInput(PORT comPort)
{
	uint32_t dwFlags = PURGE_RXCLEAR;

	BOOL status = PurgeComm(comPort, dwFlags);
	if (status == FALSE)
		return FALSE;

	return TRUE;
}

int FlushOutput(PORT comPort)
{
	uint32_t dwFlags = PURGE_TXCLEAR;

	BOOL status = PurgeComm(comPort, dwFlags);
	if (status == FALSE)
		return FALSE;

	return TRUE;
}
