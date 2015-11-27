#include "pebble.h"

#include "Logging.h"
#include "MiniDungeon.h"

// Right justified
void IntToString(char *buffer, size_t bufferSize, int value)
{
	int i = bufferSize - 1;
	int digit;
	int temp = value;

	DEBUG_VERBOSE_LOG("IntToString(%d)", value);
	
	do
	{
		digit = temp % 10;
		buffer[i] = '0' + digit;
		temp /= 10;
		--i;
	} while(temp);

	while(i >= 0)
	{
#if PAD_WITH_SPACES
		buffer[i] = ' ';
#else
		buffer[i] = '0';
#endif
		--i;
	}
}

void IntToPercent(char *buffer, size_t bufferSize, int value)
{
	buffer[bufferSize - 1] = '%';
	IntToString(buffer, bufferSize - 1, value);
}

// Returns an integer in the range [1,max]
int Random(int max)
{
	int result = (uint16_t)(rand() % max) + 1;
	DEBUG_VERBOSE_LOG("Random(%d)=%d", max, result);
	return result;
}

