#ifndef HELPER_FUNCTIONALITY_H
#define HELPER_FUNCTIONALITY_H

#include <iostream>

// Return Values
#define SUCCESS  0
#define ADDRESS_ERROR 1
#define BIND_ERROR 2
#define CONNECT_ERROR 3
#define DISCONNECT 4
#define MESSAGE_ERROR 5
#define PARAMETER_ERROR 6
#define SETUP_ERROR 7
#define SHUTDOWN 8
#define STARTUP_ERROR 9

#define OFF_BY_ONE 1

typedef unsigned char byte;
typedef unsigned short ushort;

namespace HelperFunctionality
{
	int GetUserInt(const char* _prompt, int _max, int _min = 0);
	byte GetUserString(const char* _prompt, char* _buffer, int _bufferLength);
	int GetUserSelection(const char* _prompt, const char** _options, int _numberOfOptions);
	void GetValidIPAddress(char* _buffer, int _bufferLength);
	bool IPIsValid(const char* _buffer);
}
#endif