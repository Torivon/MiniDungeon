#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "MiniDungeon.h"

// Right justified
void IntToString(char *buffer, size_t bufferSize, int value)
{
	int i = bufferSize - 1;
	int digit;
	int temp = value;

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

// Pulled from the pebble developer forums
// forums.getpebble.com/discussion/comment/28908/#comment_28908
int Random(int max)
{
	return (uint16_t)(rand() % max);
}

