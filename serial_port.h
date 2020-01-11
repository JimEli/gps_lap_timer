#ifndef _CSERIAL_PORT_
#define _CSERIAL_PORT_

#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#define CP_BAUD_RATE_9600 CBR_9600
#define CP_BAUD_RATE_1155 1155
#define CP_BAUD_RATE_4800 CBR_4800
#define CP_BAUD_RATE_19200 CBR_19200

#define CP_DATA_BITS_5 5
#define CP_DATA_BITS_6 6
#define CP_DATA_BITS_7 7
#define CP_DATA_BITS_8 8

#define CP_STOP_BITS_ONE ONESTOPBIT
#define CP_STOP_BITS_TWO TWOSTOPBITS
#define CP_STOP_BITS_ONE_AND_HALF ONE5STOPBITS

#define CP_PARITY_NOPARITY NOPARITY
#define CP_PARITY_ODD ODDPARITY
#define CP_PARITY_EVEN EVENPARITY
#define CP_PARITY_MARK MARKPARITY
#define CP_PARITY_SPACE SPACEPARITY

typedef HANDLE PORT;

// Open comm serial port specified by idx.
// Returns a handle to the opend port, on error returns NULL.
PORT OpenPort(int idx);

// Close the handle of the opend port.
void ClosePort(PORT com_port);

// Set the baud rate of the specified com_port.
// Returns TRUE if operation successful otherwise returns FALSE.
// NOTE: You can use the values of CP_BAUD_RATE to feed the rate parameter.
int SetPortBaudRate(PORT com_port, int rate);

// Set the data bits of the specified com_port.
// Returns TRUE if operation successful otherwise returns FALSE.
// NOTE: You can use the values of CP_DATA_BITS to feed the databits parameter.
// NOTE: Becareful what you send if you set this other than 8 bits.
int SetPortDataBits(PORT com_port, int databits);

// Set the stop bits of the specified com_port.
// Returns TRUE if operation successful otherwise returns FALSE.
// NOTE: Use the values of CP_STOP_BITS to feed the stopbits parameter.
int SetPortStopBits(PORT com_port, int stopbits);

// Set the parity type of the specified com_port.
// Returns TRUE if operation successful otherwise returns FALSE.
// NOTE: Use the values of CP_PARITY to feed the parity parameter.
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