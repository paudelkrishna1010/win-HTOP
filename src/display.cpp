#include <windows.h>
#include <cstdlib>
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