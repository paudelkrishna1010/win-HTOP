#include <wincon.h>
#include <psapi.h>

bool getProcessMemoryUsage(unsigned long *processIDArray, PROCESS_MEMORY_COUNTERS *memoryUsageArray, unsigned long processCount);