#pragma once
#include <minwinbase.h>

bool getProcessCpuTime(unsigned long *processIDArray, unsigned long processCount, FILETIME *timeUsage);

void FileTimeDifference(FILETIME *TimeDiff, FILETIME *prevTime, FILETIME *currentTime);

void getSystemTime(FILETIME *timeUsage);

void calcCpuUsage(FILETIME *prevTimeUsage, FILETIME *currentTimeUsage, FILETIME *prevSysTime, FILETIME *currentSysTime, double *cpuUsage, unsigned long processCount, int cpuCount);