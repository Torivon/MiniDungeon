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

static long random_seed = 100;

void SetRandomSeed(long newSeed)
{
	random_seed = newSeed;
}

// Pulled from the pebble developer forums
// forums.getpebble.com/discussion/comment/28908/#comment_28908
int Random(int max)
{
	random_seed = (((random_seed * 214013L + 2531011L) >> 16) & 32767);

	return ((random_seed % max) + 1);
}

#define TIME_ZONE_OFFSET 8

unsigned int GetUnixTime(PblTm *current_time)
{
	unsigned int unix_time;

	unix_time = ((0-TIME_ZONE_OFFSET)*3600)
		+ current_time->tm_sec
		+ current_time->tm_min*60
		+ current_time->tm_hour*3600
		+ current_time->tm_yday*86400
		+ (current_time->tm_year-70)*31536000
		+ ((current_time->tm_year-69)/4)*86400
		- ((current_time->tm_year-1)/100)*86400
		+ ((current_time->tm_year+299)/400)*86400;

	return unix_time;    
}
