#pragma once
#include <minwinbase.h>

void FileTimeDifference(FILETIME *TimeDiff, FILETIME *prevTime, FILETIME *currentTime);

void getSystemTime(FILETIME *timeUsage);

void FileTimeAddition(FILETIME *timeSum, FILETIME *prevTime, FILETIME *currentTime);

void fetchTimeUsage(unsigned long processCount);

void fetchCpuUsage(unsigned long processCount, int cpuCount, FILETIME *prevSysTime, FILETIME *currentSysTime);