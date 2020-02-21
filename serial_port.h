// Acceptable values for the DCB struct discussed here: 
// https://docs.microsoft.com/en-us/windows/win32/api/winbase/ns-winbase-dcb
// CBR_19200

#ifndef _CSERIAL_PORT_
#define _CSERIAL_PORT_

#include <Windows.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef HANDLE PORT; //typedef void* PORT;

// Open comm serial port specified by idx.
// Returns a handle to the opend port, on error returns NULL.
PORT OpenPort(unsigned portNumber);

// Close the handle of the opend port.
void ClosePort(PORT comPort);

// Set the baud rate of the specified comPort.
// Returns TRUE if operation successful otherwise returns FALSE.
int SetBaudRate(PORT comPort, int rate);

// Set the data bits of the specified comPort.
// Returns TRUE if operation successful otherwise returns FALSE.
int SetDataBits(PORT comPort, int bits);

// Set the stop bits of the specified comPort.
// Returns TRUE if operation successful otherwise returns FALSE.
int SetStopBits(PORT comPort, int bits);

// Set the parity type of the specified comPort.
// Returns TRUE if operation successful otherwise returns FALSE.
int SetParity(PORT comPort, int parity);

// Get the baud rate of the specified comPort.
int GetBaudRate(PORT comPort);

// Get the data bits of the specified comPort.
int GetDataBits(PORT comPort);

// Get the stop bits of the specified comPort.
int GetStopBits(PORT comPort);

// Get the parity of the specified comPort.
int GetParity(PORT comPort);

// Send character data to comPort.
int SendData(PORT comPort, unsigned char* data, size_t size);

// Receive character data from comPort.
int ReceiveData(PORT comPort, char * data, int len);

// Flush input data for port.
int FlushInput(PORT comPort);

// Flush output data for port.
int FlushOutput(PORT comPort);

#ifdef __cplusplus
}
#endif

#endif
