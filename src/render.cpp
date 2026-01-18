#include <windows.h>

CHAR_INFO *createFrameBuffer(short width, short height)
{
    CHAR_INFO *frame = new CHAR_INFO[width * height];
    return frame;
    return frame;
}

void paintFrame(
    CHAR_INFO *frameBuffer,
    short width,
    short row,
    short col,
    char *data)
{
    for (short i = 0; data[i] != '\0'; i++)
    {
        int idx = row * width + (col + i);

        frameBuffer[idx].Char.AsciiChar = data[i];
        frameBuffer[idx].Attributes =
            FOREGROUND_RED |
            FOREGROUND_GREEN |
            FOREGROUND_BLUE;
    }
}

void clearFrameBuffer(CHAR_INFO *frameBuffer, short width, short height)
{
    for (short i = 0; i < width * height; i++)
    {
        frameBuffer[i].Char.AsciiChar = ' ';
        frameBuffer[i].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
    }
}