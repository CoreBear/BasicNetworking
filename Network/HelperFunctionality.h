#ifndef HELPER_FUNCTIONALITY_H
#define HELPER_FUNCTIONALITY_H

#include <iostream>

#define OFF_BY_ONE 1

typedef unsigned char byte;
typedef unsigned short ushort;

namespace HelperFunctionality
{
	int GetUserInt(const char* _prompt, int _max, int _min = 0);
	void GetUserString(const char* _prompt, char* _buffer, int _bufferLength);
	int GetUserSelection(const char* _prompt, const char** _options, int _numberOfOptions);
	void GetValidIPAddress(char* _buffer, int _bufferLength);
	bool IPIsValid(const char* _buffer);
}
#endif