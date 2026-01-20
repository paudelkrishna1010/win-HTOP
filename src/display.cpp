#include <windows.h>
#include <cstdlib>
#include <iostream>
#include <display.h>
#include <buffer.h>
#include <process.h>
#include <render.h>
#include <memory_usage.h>
#include <io.h>
#include <cpu_usage.h>
#include <process_info.h>

bool mainLoop()
{
    FreeConsole();
    AllocConsole();

    short WIDTH = 100;
    short HEIGHT = 25;
    int scrollOffset = 0;
    int visibleRows = HEIGHT - 1;

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
    unsigned long processCount = 0;

    FILETIME prevSysTime, currentSysTime;
    int cpuCount = GetActiveProcessorCount(ALL_PROCESSOR_GROUPS);

    getSystemTime(&currentSysTime);
    prevSysTime = currentSysTime;

    processList.clear();

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

        getSystemTime(&currentSysTime);

        ProcessInfo pInfo;

        getProcessIDList(processIDArray, sizeof(processIDArray), &processCount);

        // filterProcessArray(processIDArray, processCount);

        oldList = processList;
        processList.clear();

        DWORD selfPid = GetCurrentProcessId();
        bool found = false;

        for (unsigned long i = 0; i < processCount; i++)
        {
            if (processIDArray[i] == selfPid)
            {
                found = true;
                break;
            }
        }

        if (!found && processCount < 4096)
        {
            processIDArray[processCount++] = selfPid;
        }

        for (unsigned long i = 0; i < processCount; i++)
        {
            ProcessInfo p{};
            p.pid = processIDArray[i];

            processList.push_back(p);
            strcpy(processList[processList.size() - 1].name, "[restricted]");
            fetchProcessName(processList.size() - 1);
            fetchMemoryUsage(processList.size() - 1);
            fetchTimeUsage(processList.size() - 1);
            fetchCpuUsage(processList.size() - 1, cpuCount, &prevSysTime, &currentSysTime);

            processList.back().prevCpuTime = processList.back().currCpuTime;
        }

        if (scrollOffset < 0)
            scrollOffset = 0;
        if (scrollOffset > (int)processList.size() - visibleRows)
            scrollOffset = processList.size() - visibleRows;
        if (scrollOffset < 0)
            scrollOffset = 0;

        prevSysTime = currentSysTime;

        clearFrameBuffer(frameBuffer, WIDTH, HEIGHT);

        paintFrame(frameBuffer, 100, 0, 0, (char *)"PID");
        paintFrame(frameBuffer, 100, 0, 10, (char *)"Name");
        paintFrame(frameBuffer, 100, 0, 50, (char *)"Memory");
        paintFrame(frameBuffer, 100, 0, 60, (char *)"CPU");

        for (int row = 0; row < visibleRows; row++)
        {
            int idx = row + scrollOffset;
            if (idx >= (int)processList.size())
                break;

            char pidStr[16];
            sprintf(pidStr, "%lu", processList[idx].pid);

            char memStrMB[16];
            sprintf(memStrMB, "%llu",
                    (unsigned long long)(processList[idx].memoryMB));

            char cpuUsageStr[16];
            sprintf(cpuUsageStr, "%.2f", processList[idx].cpuUsage);

            paintFrame(frameBuffer, WIDTH, row + 1, 0, pidStr);
            paintFrame(frameBuffer, WIDTH, row + 1, 10, processList[idx].name);
            paintFrame(frameBuffer, WIDTH, row + 1, 50, memStrMB);
            paintFrame(frameBuffer, WIDTH, row + 1, 55, (char *)"MB");
            paintFrame(frameBuffer, WIDTH, row + 1, 60, (char *)cpuUsageStr);
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
