#pragma once

bool getProcessIDList(unsigned long *processIDArray, unsigned long arraySizeInBytes, unsigned long *processCount);

void filterProcessArray(unsigned long *processIDArray, unsigned long &processCount);

void fetchProcessName(
    unsigned long processCount);

void fetchPriority(
    unsigned long index);