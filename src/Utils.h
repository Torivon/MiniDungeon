#pragma once

void IntToString(char *buffer, size_t bufferSize, int value);
int Random(int max);
void SetRandomSeed(long newSeed);

unsigned int GetUnixTime(PblTm *current_time);
