#pragma once
#include <wincon.h>

void *createConsoleBuffer();
bool writeFrameToConsoleBuffer(
    HANDLE consoleBuffer,
    CHAR_INFO *data,
    short width,
    short height);
bool setConsoleBufferActive(void *consoleBuffer);