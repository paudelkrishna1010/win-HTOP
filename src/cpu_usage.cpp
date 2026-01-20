#include <windows.h>
#include <psapi.h>
#include <cpu_usage.h>

bool getProcessCpuTime(unsigned long *processIDArray, unsigned long processCount, FILETIME *timeUsage)
{
    FILETIME kernelTime;
    FILETIME userTime;

    for (unsigned long i = 0; i < processCount; i++)
    {
        HANDLE processHandle = OpenProcess(
            PROCESS_QUERY_LIMITED_INFORMATION,
            FALSE,
            processIDArray[i]);

        kernelTime.dwHighDateTime = 0;
        kernelTime.dwLowDateTime = 0;
        userTime.dwHighDateTime = 0;
        userTime.dwLowDateTime = 0;

        if (!processHandle)
            continue;

        FILETIME creationTime, exitTime;
        BOOL ok = GetProcessTimes(processHandle, &creationTime, &exitTime, &kernelTime, &userTime);

        if (!ok)
        {
            kernelTime.dwHighDateTime = 0;
            kernelTime.dwLowDateTime = 0;
            userTime.dwHighDateTime = 0;
            userTime.dwLowDateTime = 0;
        }

        ULARGE_INTEGER timeUsageInt, kernelTimeInt, userTimeInt;

        kernelTimeInt.HighPart = kernelTime.dwHighDateTime;
        kernelTimeInt.LowPart = kernelTime.dwLowDateTime;

        userTimeInt.HighPart = userTime.dwHighDateTime;
        userTimeInt.LowPart = userTime.dwLowDateTime;

        timeUsageInt.QuadPart = kernelTimeInt.QuadPart + userTimeInt.QuadPart;

        timeUsage[i].dwHighDateTime = timeUsageInt.HighPart;
        timeUsage[i].dwLowDateTime = timeUsageInt.LowPart;

        CloseHandle(processHandle);
    }

    return true;
}

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

void calcCpuUsage(FILETIME *prevTimeUsage, FILETIME *currentTimeUsage, FILETIME *prevSysTime, FILETIME *currentSysTime, double *cpuUsage, unsigned long processCount, int cpuCount)
{

    FILETIME processTimeDifference;
    FILETIME sysTimeDifference;
    FileTimeDifference(&sysTimeDifference, prevSysTime, currentSysTime);
    ULARGE_INTEGER sysTimeDelta;
    sysTimeDelta.HighPart = sysTimeDifference.dwHighDateTime;
    sysTimeDelta.LowPart = sysTimeDifference.dwLowDateTime;

    for (unsigned long i = 0; i < processCount; i++)
    {
        FileTimeDifference(&processTimeDifference, &prevTimeUsage[i], &currentTimeUsage[i]);

        ULARGE_INTEGER processTimeDelta;
        processTimeDelta.HighPart = processTimeDifference.dwHighDateTime;
        processTimeDelta.LowPart = processTimeDifference.dwLowDateTime;

        if (sysTimeDelta.QuadPart == 0)
        {
            cpuUsage[i] = 0.0;
            continue;
        }

        cpuUsage[i] = ((double)processTimeDelta.QuadPart * 100.00) / ((double)sysTimeDelta.QuadPart * cpuCount);
    }
}
