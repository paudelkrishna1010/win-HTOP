#include <io.h>
#include <windows.h>

void hideConsoleCursor(HANDLE consoleBuffer)
{
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = FALSE;

    SetConsoleCursorInfo(consoleBuffer, &cursorInfo);
}

void enableMouseInput()
{
    HANDLE handleInput = GetStdHandle(STD_INPUT_HANDLE);

    DWORD mode;
    GetConsoleMode(handleInput, &mode);

    mode |= ENABLE_MOUSE_INPUT;
    mode &= ~ENABLE_QUICK_EDIT_MODE;
    SetConsoleMode(handleInput, mode);
}