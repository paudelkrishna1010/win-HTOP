#include <sort.h>
#include <algorithm>

void sortProcessList(std::vector<ProcessInfo> &processList, SortType sortType, SortOrder sortOrder)
{
    switch (sortType)
    {
    case PID:
        std::sort(processList.begin(), processList.end(), [sortOrder](const ProcessInfo &a, const ProcessInfo &b)
                  { return (sortOrder == ASCENDING) ? (a.pid < b.pid) : (a.pid > b.pid); });
        break;
    case NAME:
        std::sort(processList.begin(), processList.end(), [sortOrder](const ProcessInfo &a, const ProcessInfo &b)
                  { char aName[1024], bName[1024];
                    strcpy(aName, a.name);
                    strcpy(bName, b.name);
                    int i = 0;
                    while(aName[i] != '\0' && bName[i] != '\0')
                    {
                        aName[i] = std::tolower(aName[i]);
                        bName[i] = std::tolower(bName[i]);
                        i++;
                    }
                    while(aName[i] != '\0')
                    {
                        aName[i] = std::tolower(aName[i]);
                        i++;
                    }
                    while(bName[i] != '\0')
                    {
                        bName[i] = std::tolower(bName[i]);
                        i++;
                    }
                    int result = strcmp(aName, bName);
                    return (sortOrder == ASCENDING) ? (result < 0) : (result > 0); });
        break;
    case MEMORY:
        std::sort(processList.begin(), processList.end(), [sortOrder](const ProcessInfo &a, const ProcessInfo &b)
                  { return (sortOrder == ASCENDING) ? a.memoryMB < b.memoryMB : a.memoryMB > b.memoryMB; });
        break;
    case CPU:
        std::sort(processList.begin(), processList.end(), [sortOrder](const ProcessInfo &a, const ProcessInfo &b)
                  { return (sortOrder == ASCENDING) ? a.cpuUsage < b.cpuUsage : a.cpuUsage > b.cpuUsage; });
        break;
    }
}
