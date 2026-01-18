#include <windows.h>
#include <cstdlib>
#include <iostream>
#include <display.h>
#include <buffer.h>
#include <process.h>
#include <render.h>

bool mainLoop()
{
    HANDLE buffer1 = createConsoleBuffer();
    HANDLE buffer2 = createConsoleBuffer();

    unsigned long processIDArray[4096];
    unsigned long processCount;
    getProcessIDList(processIDArray, sizeof(processIDArray), processCount);

    char *processNameArray[4096];
    getProcessNameList(processIDArray, processCount, processNameArray);

    CHAR_INFO *frameBuffer = createFrameBuffer(100, 30);
    clearFrameBuffer(frameBuffer, 100, 30);

    paintFrame(frameBuffer, 100, 0, 0, (char *)"PID");
    paintFrame(frameBuffer, 100, 0, 10, (char *)"Name");

    for (int i = 0; i < processCount && i < 29; i++)
    {
        char processIdInString[16];
        sprintf(processIdInString, "%lu", processIDArray[i]);

        paintFrame(frameBuffer, 100, i + 1, 0, processIdInString);
        paintFrame(frameBuffer, 100, i + 1, 10, processNameArray[i]);
    }
}

bool allocateConsole()
{
    if (GetConsoleCP())
    {
        return true;
    }
    else
    {
        BOOL newConsole = AllocConsole();
        if (newConsole)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}

void paintConsole(
    unsigned long processIDArray[],
    char *processNameArray[],
    unsigned long processCount)
{
    system("cls");
    std::cout.width(10);
    std::cout << std::left << "PID";
    std::cout.width(50);
    std::cout << std::left << "Name" << std::endl;

    for (int i = 0; i < processCount; i++)
    {
        std::cout.width(10);
        std::cout << std::left << processIDArray[i];
        std::cout.width(50);
        std::cout << std::left << processNameArray[i] << std::endl;
    }
}
