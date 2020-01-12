#ifndef _CSERIAL_PORT_
#define _CSERIAL_PORT_

#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef HANDLE PORT;

// Open comm serial port specified by idx.
// Returns a handle to the opend port, on error returns NULL.
PORT OpenPort(unsigned idx);

// Close the handle of the opend port.
void ClosePort(PORT com_port);

// Set the baud rate of the specified com_port.
// Returns TRUE if operation successful otherwise returns FALSE.
int SetPortBaudRate(PORT com_port, int rate);

// Set the data bits of the specified com_port.
// Returns TRUE if operation successful otherwise returns FALSE.
int SetPortDataBits(PORT com_port, int databits);

// Set the stop bits of the specified com_port.
// Returns TRUE if operation successful otherwise returns FALSE.
int SetPortStopBits(PORT com_port, int stopbits);

// Set the parity type of the specified com_port.
// Returns TRUE if operation successful otherwise returns FALSE.
int SetPortParity(PORT com_port, int parity);

// Get the baud rate of the specified com_port.
int GetPortBaudRate(PORT com_port);

// Get the data bits of the specified com_port.
int GetPortDataBits(PORT com_port);

// Get the stop bits of the specified com_port.
int GetPortStopBits(PORT com_port);

// Get the parity of the specified com_port.
int GetPortParity(PORT com_port);

// Send character data to com_port.
int SendData(PORT com_port, unsigned char* data, size_t size);

// Receive character data from com_port.
int ReceiveData(PORT com_port, char * databuffer, int bufferlen);

#ifdef __cplusplus
}
#endif

#endif
