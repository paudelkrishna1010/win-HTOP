#include <windows.h>
#include <psapi.h>
#include <cpu_usage.h>
#include <process_info.h>

void FileTimeDifference(FILETIME *TimeDiff, FILETIME *prevTime, FILETIME *currentTime)
{
    ULARGE_INTEGER prevTimeInt, currentTimeInt, timeDiffInt;

    prevTimeInt.HighPart = prevTime->dwHighDateTime;
    prevTimeInt.LowPart = prevTime->dwLowDateTime;

    currentTimeInt.HighPart = currentTime->dwHighDateTime;
    currentTimeInt.LowPart = currentTime->dwLowDateTime;

    timeDiffInt.QuadPart = currentTimeInt.QuadPart - prevTimeInt.QuadPart;
    TimeDiff->dwHighDateTime = timeDiffInt.HighPart;
    TimeDiff->dwLowDateTime = timeDiffInt.LowPart;
}

void getSystemTime(FILETIME *timeUsage)
{
    FILETIME idleTime, kernelTime, userTime;
    GetSystemTimes(&idleTime, &kernelTime, &userTime);

    ULARGE_INTEGER idleTimeInt, kernelTimeInt, userTimeInt;

    idleTimeInt.HighPart = idleTime.dwHighDateTime;
    idleTimeInt.LowPart = idleTime.dwLowDateTime;

    kernelTimeInt.HighPart = kernelTime.dwHighDateTime;
    kernelTimeInt.LowPart = kernelTime.dwLowDateTime;

    userTimeInt.HighPart = userTime.dwHighDateTime;
    userTimeInt.LowPart = userTime.dwLowDateTime;

    ULARGE_INTEGER timeUsageInt;
    timeUsageInt.QuadPart = kernelTimeInt.QuadPart + userTimeInt.QuadPart;

    timeUsage->dwHighDateTime = timeUsageInt.HighPart;
    timeUsage->dwLowDateTime = timeUsageInt.LowPart;
}

void FileTimeAddition(FILETIME *timeSum, FILETIME *prevTime, FILETIME *currentTime)
{
    ULARGE_INTEGER prevTimeInt, currentTimeInt, timeSumInt;

    prevTimeInt.HighPart = prevTime->dwHighDateTime;
    prevTimeInt.LowPart = prevTime->dwLowDateTime;

    currentTimeInt.HighPart = currentTime->dwHighDateTime;
    currentTimeInt.LowPart = currentTime->dwLowDateTime;

    timeSumInt.QuadPart = currentTimeInt.QuadPart + prevTimeInt.QuadPart;
    timeSum->dwHighDateTime = timeSumInt.HighPart;
    timeSum->dwLowDateTime = timeSumInt.LowPart;
}

void fetchTimeUsage(unsigned long processCount)
{
    FILETIME processKernelTime, processUserTime, creationTime, exitTime;

    HANDLE processHandle = OpenProcess(
        PROCESS_QUERY_LIMITED_INFORMATION,
        FALSE,
        processList[processCount].pid);

    if (!processHandle)
    {
        processList[processCount].currCpuTime.dwHighDateTime = 0;
        processList[processCount].currCpuTime.dwLowDateTime = 0;
        return;
    }

    BOOL success = GetProcessTimes(processHandle, &creationTime, &exitTime, &processKernelTime, &processUserTime);

    if (!success)
    {
        processList[processCount].currCpuTime.dwHighDateTime = 0;
        processList[processCount].currCpuTime.dwLowDateTime = 0;
        CloseHandle(processHandle);
        return;
    }

    FileTimeAddition(&processList[processCount].currCpuTime, &processKernelTime, &processUserTime);

    CloseHandle(processHandle);
}

void fetchCpuUsage(unsigned long processCount, int cpuCount, FILETIME *prevSysTime, FILETIME *currentSysTime)
{
    FILETIME processTimeDelta, systemTimeDelta;

    FileTimeDifference(&systemTimeDelta, prevSysTime, currentSysTime);

    for (auto &p : processList)
    {
        bool found = false;
        for (auto &o : oldList)
        {
            if (p.pid == o.pid)
            {
                found = true;
                p.prevCpuTime = o.currCpuTime;
                break;
            }
        }

        if (!found)
        {
            ZeroMemory(&p.prevCpuTime, sizeof(FILETIME));
        }
    }

    FileTimeDifference(&processTimeDelta, &processList[processCount].prevCpuTime, &processList[processCount].currCpuTime);

    ULARGE_INTEGER processTimeDeltaInt, systemTimeDeltaInt;

    processTimeDeltaInt.HighPart = processTimeDelta.dwHighDateTime;
    processTimeDeltaInt.LowPart = processTimeDelta.dwLowDateTime;

    systemTimeDeltaInt.HighPart = systemTimeDelta.dwHighDateTime;
    systemTimeDeltaInt.LowPart = systemTimeDelta.dwLowDateTime;

    if (systemTimeDeltaInt.QuadPart == 0)
    {
        processList[processCount].cpuUsage = 0.0;
        return;
    }

    double newValue =
        ((double)processTimeDeltaInt.QuadPart * 100.00) /
        ((double)systemTimeDeltaInt.QuadPart);

    processList[processCount].cpuUsage =
        processList[processCount].cpuUsage * 0.7 + newValue * 0.3;
}