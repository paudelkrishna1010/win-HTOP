#include <windows.h>
#include <iostream>

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
    unsigned long processCount)
{
    std::cout << "Process ID List: " << std::endl;
    for (int i = 0; i < processCount; i++)
    {
        std::cout << processIDArray[i] << std::endl;
    }
}