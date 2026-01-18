#include <windows.h>
#include <cstdlib>
#include <iostream>
#include <display.h>
#include <buffer.h>
#include <process.h>
#include <render.h>

bool mainLoop()
{
    FreeConsole();
    AllocConsole();

    int scrollOffset = 0;
    int visibleRows = 29;

    HANDLE buffer1 = createConsoleBuffer();
    HANDLE buffer2 = createConsoleBuffer();

    HANDLE activeBuffer = buffer1;
    HANDLE backBuffer = buffer2;

    CHAR_INFO *frameBuffer = createFrameBuffer(100, 30);

    COORD size = {100, 30};
    SMALL_RECT window = {0, 0, 99, 29};

    SetConsoleScreenBufferSize(activeBuffer, size);
    SetConsoleWindowInfo(activeBuffer, TRUE, &window);
    setConsoleBufferActive(activeBuffer);

    unsigned long processIDArray[4096];
    char *processNameArray[4096];
    unsigned long processCount = 0;

    while (true)
    {

        if (GetAsyncKeyState(VK_UP) & 0x8000)
            scrollOffset--;
        if (GetAsyncKeyState(VK_DOWN) & 0x8000)
            scrollOffset++;
        if (GetAsyncKeyState('Q') & 0x8000)
        {
            delete[] frameBuffer;
            CloseHandle(buffer1);
            CloseHandle(buffer2);
            break;
        }

        getProcessIDList(processIDArray, sizeof(processIDArray), processCount);

        getProcessNameList(processIDArray, processCount, processNameArray);

        if (scrollOffset < 0)
            scrollOffset = 0;
        if (scrollOffset > (int)processCount - visibleRows)
            scrollOffset = processCount - visibleRows;
        if (scrollOffset < 0)
            scrollOffset = 0;

        clearFrameBuffer(frameBuffer, 100, 30);

        paintFrame(frameBuffer, 100, 0, 0, (char *)"PID");
        paintFrame(frameBuffer, 100, 0, 10, (char *)"Name");

        for (int row = 0; row < visibleRows; row++)
        {
            int idx = row + scrollOffset;
            if (idx >= processCount)
                break;

            char pidStr[16];
            sprintf(pidStr, "%lu", processIDArray[idx]);

            paintFrame(frameBuffer, 100, row + 1, 0, pidStr);
            paintFrame(frameBuffer, 100, row + 1, 10, processNameArray[idx]);
        }

        writeFrameToConsoleBuffer(backBuffer, frameBuffer, 100, 30);

        // swap buffers
        setConsoleBufferActive(backBuffer);

        HANDLE tmp = activeBuffer;
        activeBuffer = backBuffer;
        backBuffer = tmp;

        Sleep(50);
    }

    return true;
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
