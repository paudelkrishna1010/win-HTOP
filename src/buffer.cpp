#include <windows.h>

void *createConsoleBuffer()
{
    return CreateConsoleScreenBuffer(GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
}

bool writeFrameToConsoleBuffer(void *consoleBuffer, CHAR_INFO *data)
{
    return WriteConsoleOutput(consoleBuffer, data, {100, 30}, {0, 0}, NULL);
}

bool setConsoleBufferActive(void *consoleBuffer)
{
    return SetConsoleActiveScreenBuffer(consoleBuffer);
}