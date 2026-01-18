#include <process.h>
#include <display.h>

int main()
{

    // unsigned long processIDArray[4096];
    // unsigned long processCount;
    // getProcessIDList(processIDArray, sizeof(processIDArray), processCount);

    // char *processNameArray[4096];
    // getProcessNameList(processIDArray, processCount, processNameArray);

    // allocateConsole();
    // paintConsole(processIDArray, processNameArray, processCount);

    mainLoop();

    return 0;
}