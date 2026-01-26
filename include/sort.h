#pragma once
#include <process_info.h>

enum SortType
{
    PID,
    NAME,
    MEMORY,
    CPU
};

enum SortOrder
{
    ASCENDING,
    DESCENDING
};

void sortProcessList(std::vector<ProcessInfo> &processList, SortType sortType, SortOrder sortOrder);
