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

// Change function signature to accept a handle
void getConsoleSize(HANDLE hActive, short *width, short *height)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(hActive, &csbi))
    {
        *width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
        *height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }
    else
    {
        *width = 100;
        *height = 25;
    }
}

bool mainLoop()
{
    FreeConsole();
    AllocConsole();

    short WIDTH = 100;
    short HEIGHT = 25;
    int scrollOffset = 0;

    HANDLE buffer1 = createConsoleBuffer();
    HANDLE buffer2 = createConsoleBuffer();

    HANDLE activeBuffer = buffer1;
    HANDLE backBuffer = buffer2;

    getConsoleSize(activeBuffer, &WIDTH, &HEIGHT);
    int visibleRows = HEIGHT - 1;

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

    ULONGLONG lastCpuUpdateTime = 0;
    bool updateCpu = false;

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

        while (events > 0)
        {
            INPUT_RECORD record;
            DWORD read;

            ReadConsoleInput(handleInput, &record, 1, &read);
            events--;

            if (record.EventType == WINDOW_BUFFER_SIZE_EVENT)
            {
                getConsoleSize(activeBuffer, &WIDTH, &HEIGHT);
                visibleRows = HEIGHT - 1;

                if (frameBuffer)
                    delete[] frameBuffer;
                frameBuffer = createFrameBuffer(WIDTH, HEIGHT);

                COORD newSize = {WIDTH, HEIGHT};
                SMALL_RECT newWindow = {0, 0, WIDTH - 1, HEIGHT - 1};

                auto SafeResize = [&](HANDLE hBuf)
                {
                    CONSOLE_SCREEN_BUFFER_INFO csbi;
                    GetConsoleScreenBufferInfo(hBuf, &csbi);

                    bool shrinking = (WIDTH < csbi.dwSize.X) || (HEIGHT < csbi.dwSize.Y);

                    if (shrinking)
                    {
                        if (!SetConsoleWindowInfo(hBuf, TRUE, &newWindow))
                        {
                            SMALL_RECT minimal = {0, 0, 1, 1};
                            SetConsoleWindowInfo(hBuf, TRUE, &minimal);
                            SetConsoleWindowInfo(hBuf, TRUE, &newWindow);
                        }
                        SetConsoleScreenBufferSize(hBuf, newSize);
                    }
                    else
                    {
                        SetConsoleScreenBufferSize(hBuf, newSize);
                        SetConsoleWindowInfo(hBuf, TRUE, &newWindow);
                    }
                };

                SafeResize(buffer1);
                SafeResize(buffer2);
            }

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

                    int maxScroll = (int)processList.size() - visibleRows;
                    if (maxScroll < 0)
                        maxScroll = 0;

                    if (scrollOffset > maxScroll)
                        scrollOffset = maxScroll;
                }
            }
        }

        ProcessInfo pInfo;

        getProcessIDList(processIDArray, sizeof(processIDArray), &processCount);

        filterProcessArray(processIDArray, processCount);

        ULONGLONG now = GetTickCount64();
        if (now - lastCpuUpdateTime > 1000)
        {
            updateCpu = true;
            getSystemTime(&currentSysTime);
        }
        else
        {
            updateCpu = false;
        }

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
            ProcessInfo &newProc = processList.back();

            bool foundOld = false;
            for (auto &oldProc : oldList)
            {
                if (oldProc.pid == newProc.pid)
                {
                    strcpy(newProc.name, oldProc.name);
                    newProc.prevCpuTime = oldProc.prevCpuTime;
                    newProc.cpuUsage = oldProc.cpuUsage;
                    foundOld = true;
                    break;
                }
            }

            if (!foundOld)
            {
                strcpy(newProc.name, "[restricted]");
                fetchProcessName(processList.size() - 1);

                fetchTimeUsage(processList.size() - 1);
                newProc.prevCpuTime = newProc.currCpuTime;
                newProc.cpuUsage = 0.0;
            }

            fetchMemoryUsage(processList.size() - 1);

            if (updateCpu)
            {
                fetchTimeUsage(processList.size() - 1);

                fetchCpuUsage(processList.size() - 1, cpuCount, &prevSysTime, &currentSysTime);

                newProc.prevCpuTime = newProc.currCpuTime;
            }
        }

        if (scrollOffset < 0)
            scrollOffset = 0;
        int maxScroll = (int)processList.size() - visibleRows;
        if (maxScroll < 0)
            maxScroll = 0;
        if (scrollOffset > maxScroll)
            scrollOffset = maxScroll;

        if (scrollOffset < 0)
            scrollOffset = 0;

        // prevSysTime = currentSysTime;
        if (updateCpu)
        {
            prevSysTime = currentSysTime;
            lastCpuUpdateTime = now;
        }

        clearFrameBuffer(frameBuffer, WIDTH, HEIGHT);

        paintFrame(frameBuffer, WIDTH, 0, 0, (char *)"PID");
        paintFrame(frameBuffer, WIDTH, 0, 10, (char *)"Name");
        paintFrame(frameBuffer, WIDTH, 0, 50, (char *)"Memory");
        paintFrame(frameBuffer, WIDTH, 0, 60, (char *)"CPU");

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
