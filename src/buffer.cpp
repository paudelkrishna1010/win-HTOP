#include <windows.h>

void *createConsoleBuffer()
{
    return CreateConsoleScreenBuffer(
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        CONSOLE_TEXTMODE_BUFFER,
        NULL);
}

bool writeFrameToConsoleBuffer(
    HANDLE consoleBuffer,
    CHAR_INFO *data,
    short width,
    short height)
{
    COORD bufferSize = {width, height};
    COORD bufferCoord = {0, 0};

    SMALL_RECT writeRegion = {
        0,
        0,
        static_cast<SHORT>(width - 1),
        static_cast<SHORT>(height - 1)};

    BOOL ok = WriteConsoleOutput(
        consoleBuffer,
        data,
        bufferSize,
        bufferCoord,
        &writeRegion);

    return ok;
}

bool setConsoleBufferActive(void *consoleBuffer)
{
    return SetConsoleActiveScreenBuffer(consoleBuffer);
}