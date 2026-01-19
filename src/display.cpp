#include <windows.h>
#include <cstdlib>
#include <iostream>
#include <display.h>
#include <buffer.h>
#include <process.h>
#include <render.h>
#include <memory.h>
#include <io.h>

bool mainLoop()
{
    FreeConsole();
    AllocConsole();

    short WIDTH = 100;
    short HEIGHT = 50;
    int scrollOffset = 0;
    int visibleRows = 49;

    HANDLE buffer1 = createConsoleBuffer();
    HANDLE buffer2 = createConsoleBuffer();

    HANDLE activeBuffer = buffer1;
    HANDLE backBuffer = buffer2;

    CHAR_INFO *frameBuffer = createFrameBuffer(WIDTH, HEIGHT);

    COORD size = {WIDTH, HEIGHT};
    SMALL_RECT window = {0, 0, WIDTH - 1, HEIGHT - 1};

    SetConsoleScreenBufferSize(activeBuffer, size);
    SetConsoleWindowInfo(activeBuffer, TRUE, &window);
    setConsoleBufferActive(activeBuffer);

    unsigned long processIDArray[4096];
    char **processNameArray = new char *[4096];
    unsigned long processCount = 0;
    PROCESS_MEMORY_COUNTERS memoryUsageArray[4096];

    for (int i = 0; i < 4096; i++)
    {
        processNameArray[i] = nullptr;
    }

    while (true)
    {

        for (unsigned long i = 0; i < processCount; i++)
        {
            delete[] processNameArray[i];
            processNameArray[i] = nullptr;
        }

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

        HANDLE handleInput = GetStdHandle(STD_INPUT_HANDLE);
        DWORD events;
        GetNumberOfConsoleInputEvents(handleInput, &events);

        if (events > 0)
        {
            INPUT_RECORD record;
            DWORD read;

            ReadConsoleInput(handleInput, &record, 1, &read);

            if (record.EventType == MOUSE_EVENT)
            {
                MOUSE_EVENT_RECORD &mouse = record.Event.MouseEvent;

                if (mouse.dwEventFlags == MOUSE_WHEELED)
                {
                    SHORT delta = HIWORD(mouse.dwButtonState);

                    if (delta > 0)
                        scrollOffset--;
                    else
                        scrollOffset++;
                }
            }
        }

        if (scrollOffset < 0)
            scrollOffset = 0;
        if (scrollOffset > (int)processCount - visibleRows)
            scrollOffset = processCount - visibleRows;
        if (scrollOffset < 0)
            scrollOffset = 0;

        getProcessIDList(processIDArray, sizeof(processIDArray), processCount);
        // filterProcessArray(processIDArray, processCount);

        getProcessNameList(processIDArray, processCount, processNameArray);

        getProcessMemoryUsage(processIDArray, memoryUsageArray, processCount);

        clearFrameBuffer(frameBuffer, WIDTH, HEIGHT);

        paintFrame(frameBuffer, 100, 0, 0, (char *)"PID");
        paintFrame(frameBuffer, 100, 0, 10, (char *)"Name");
        paintFrame(frameBuffer, 100, 0, 50, (char *)"Memory");

        for (int row = 0; row < visibleRows; row++)
        {
            int idx = row + scrollOffset;
            if (idx >= processCount)
                break;

            char pidStr[16];
            sprintf(pidStr, "%lu", processIDArray[idx]);

            char memStrMB[16];
            sprintf(memStrMB, "%lu",
                    (unsigned long)(memoryUsageArray[idx].WorkingSetSize / (1024 * 1024)));

            paintFrame(frameBuffer, WIDTH, row + 1, 0, pidStr);
            paintFrame(frameBuffer, WIDTH, row + 1, 10, processNameArray[idx]);
            paintFrame(frameBuffer, WIDTH, row + 1, 50, memStrMB);
            paintFrame(frameBuffer, WIDTH, row + 1, 55, (char*)"MB");
        }

        writeFrameToConsoleBuffer(backBuffer, frameBuffer, WIDTH, HEIGHT);

        // swap buffers
        setConsoleBufferActive(backBuffer);
        hideConsoleCursor(activeBuffer);
        enableMouseInput();

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
