#include "process.h"
#include "display.h"

int main()
{
    unsigned long processIDArray[4096];
    unsigned long processCount;
    getProcessIDList(processIDArray, sizeof(processIDArray), processCount);
    allocateConsole();
    paintConsole(processIDArray, processCount);

    return 0;
}