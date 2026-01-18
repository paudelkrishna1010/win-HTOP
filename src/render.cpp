#include <windows.h>

CHAR_INFO* createFrameBuffer(short width, short height){
    CHAR_INFO frame[width*height];
    return frame;
}

void paintFrame(CHAR_INFO *frameBuffer, short width, short row, short col, char *data){
    for (short i = 0; i < sizeof(data); i++)
    {
        frameBuffer[row*width+col].Char.AsciiChar = data[i];
    }   
}

void clearFrameBuffer(CHAR_INFO *frameBuffer, short width, short height){
    for (short i = 0; i < width*height; i++)
    {
        frameBuffer[i].Char.AsciiChar = ' ';
    }
}