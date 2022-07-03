#include "HelperFunctionality.h"

int HelperFunctionality::GetUserInt(const char* _prompt, int _max, int _min)
{
	int input = 0;

	do
	{
		// Prompt user
		// If no prompt available, use generic prompt
		if (_prompt == nullptr)
		{
			std::cout << "Please enter a number between min (" << _min << ") inclusive and max (" << _max << ") inclusive: ";
		}

		// If there is a prompt, use it
		else
		{
			std::cout << _prompt << " between min(" << _min << ") inclusive and max(" << _max << ") inclusive: ";
		}

		// Get user input
		std::cin >> input;

		// If number
		if (std::cin.good())
		{
			// Valid number
			if (input >= _min && input <= _max)
			{
				break;
			}

			// Invalid number
			else
			{
				std::cout << "Invalid input! Please try again." << std::endl;
			}
		}

		// If not number
		else
		{
			std::cout << "Invalid input! Please try again." << std::endl;

			std::cin.clear();
		}

		std::cin.ignore(INT_MAX, '\n');

		// Wait for user to acknowledge
		system("pause");

	} while (true);

	std::cin.ignore(INT_MAX, '\n');

	return input;
}
byte HelperFunctionality::GetUserString(const char* _prompt, char* _buffer, int _bufferLength)
{
	// Prompt user
	std::cout << _prompt;

	std::cin.getline(_buffer, _bufferLength);

	// Off by one for the null terminator
	return static_cast<byte>(strlen(_buffer) + OFF_BY_ONE);
}
int HelperFunctionality::GetUserSelection(const char* _prompt, const char** _options, int _numberOfOptions)
{
	// Prompt user
	std::cout << _prompt << std::endl;

	constexpr int OPTIONS_START_AT_ONE = 1;

	// Display options
	for (int i = 0; i < _numberOfOptions; i++)
	{
		std::cout << (i + OPTIONS_START_AT_ONE) << ". " << _options[i] << std::endl;
	}

	return GetUserInt(nullptr, _numberOfOptions, OPTIONS_START_AT_ONE);
}
void HelperFunctionality::GetValidIPAddress(char* _buffer, int _bufferLength)
{
	do
	{
		// Get user IP address
		GetUserString("Enter IP address in dot format: ", _buffer, _bufferLength);

		// If valid, CloseSockets attempting
		if (IPIsValid(_buffer))
		{
			break;
		}

		// If invalid, inform the user and try again
		else
		{
			std::cout << "Invalid IP address! Please try again." << std::endl;

			// Wait for user to acknowledge
			system("pause");
		}
	} while (true);
}
bool HelperFunctionality::IPIsValid(const char* _buffer)
{
	char numberOfDigitsInOctet = 0, octetCounter = 1;
	constexpr char MAX_NUMBER_OF_DIGITS_IN_OCTET = 3, NUMBER_OF_OCTETS_IN_ADDRESS = 4;
	const char* walker = _buffer;
	char octetDigits[MAX_NUMBER_OF_DIGITS_IN_OCTET];

	// While not at the end of the address
	while (*walker != '\0')
	{
		switch (numberOfDigitsInOctet)
		{
		case 0:
		case 1:
		case 2:
		{
			// If a number, store the digit and increment the iterator
			if (*walker >= '0' && *walker <= '9')
			{
				octetDigits[numberOfDigitsInOctet++] = *walker;
			}

			// If a dot, move on to the first index of the next octet
			else if (*walker == '.')
			{
				++octetCounter;
				numberOfDigitsInOctet = 0;
			}

			// If anything else
			else
			{
				return false;
			}
		}
		break;
		case 3:
		{
			// If a dot
			if (*walker == '.')
			{
				// If in this case, there are three digits in this octet
				// If so, the first digit's max is 2 (i.e. 255)
				if (octetDigits[0] > '2')
				{
					return false;
				}
				
				// Move on to the first index of the next octet
				++octetCounter;
				numberOfDigitsInOctet = 0;
			}

			// If not a dot
			else
			{
				return false;
			}
		}
		break;
		default:
			return false;
			break;
		}

		// Walk forward
		++walker;
	}

	// If there are 3 digits in octet
	if (numberOfDigitsInOctet == MAX_NUMBER_OF_DIGITS_IN_OCTET)
	{
		// If so, the first digit's max is 2 (i.e. 255)
		if (octetDigits[0] > '2')
		{
			return false;
		}
	}

	// If the proper number of octets
	return (octetCounter == NUMBER_OF_OCTETS_IN_ADDRESS) ? true : false;
}