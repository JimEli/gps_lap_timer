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

int SetPortBaudRate(PORT comPort, int rate)
{
	DCB dcbSerialParams = { 0 };
	BOOL Status;

	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	Status = GetCommState(comPort, &dcbSerialParams);
	if (Status == FALSE)
		return FALSE;
	
	dcbSerialParams.BaudRate = rate;
	Status = SetCommState(comPort, &dcbSerialParams);
	return Status;
}

int SetPortDataBits(PORT comPort, int bits)
{
	DCB dcbSerialParams = { 0 };
	BOOL Status;
	
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	Status = GetCommState(comPort, &dcbSerialParams);
	if (Status == FALSE)
		return FALSE;
	
	dcbSerialParams.ByteSize = bits;
	Status = SetCommState(comPort, &dcbSerialParams);
	return Status;
}

int SetPortStopBits(PORT comPort, int bits)
{
	DCB dcbSerialParams = { 0 };
	BOOL Status;
	
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	Status = GetCommState(comPort, &dcbSerialParams);
	if (Status == FALSE)
		return FALSE;
	
	dcbSerialParams.StopBits = bits;
	Status = SetCommState(comPort, &dcbSerialParams);
	return Status;
}

int SetPortParity(PORT comPort, int parity)
{
	DCB dcbSerialParams = { 0 };
	BOOL Status;

	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	Status = GetCommState(comPort, &dcbSerialParams);
	if (Status == FALSE)
		return FALSE;
	
	dcbSerialParams.Parity = parity;
	Status = SetCommState(comPort, &dcbSerialParams);
	return Status;
}

int GetPortBaudRate(PORT comPort)
{
	DCB dcbSerialParams = { 0 };
	BOOL Status;
	
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	Status = GetCommState(comPort, &dcbSerialParams);
	if (Status == FALSE)
		return -1;
	
	return dcbSerialParams.BaudRate;
}

int GetPortDataBits(PORT comPort) 
{
	DCB dcbSerialParams = { 0 };
	BOOL Status;

	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	Status = GetCommState(comPort, &dcbSerialParams);
	if (Status == FALSE)
		return -1;
	
	return dcbSerialParams.ByteSize;
}

int GetPortStopBits(PORT comPort) 
{
	DCB dcbSerialParams = { 0 };
	BOOL Status;

	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	Status = GetCommState(comPort, &dcbSerialParams);
	if (Status == FALSE)
		return -1;
	
	return dcbSerialParams.StopBits;
}

int GetPortParity(PORT comPort) 
{
	DCB dcbSerialParams = { 0 };
	BOOL Status;

	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	Status = GetCommState(comPort, &dcbSerialParams);
	if (Status == FALSE)
		return -1;

	return dcbSerialParams.Parity;
}

int SendData(PORT comPort, unsigned char* data, size_t size)
{
	DWORD  dNoOFBytestoWrite = size;
	DWORD  dNoOfBytesWritten;
	
	BOOL Status = WriteFile(comPort, data, dNoOFBytestoWrite, &dNoOfBytesWritten, NULL);
	if (Status == FALSE)
		return -1;

	return dNoOfBytesWritten;
}

int ReceiveData(PORT comPort, char* data, int len)
{
	DWORD dwEventMask;
	DWORD NoBytesRead;

	BOOL Status = WaitCommEvent(comPort, &dwEventMask, NULL);
	if (Status == FALSE) 
		return FALSE;
	
	Status = ReadFile(comPort, data, len, &NoBytesRead, NULL);
	data[NoBytesRead] = 0;
	if (Status == FALSE) 
		return FALSE;
	
	return TRUE;
}
