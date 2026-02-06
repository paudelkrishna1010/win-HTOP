#include <user_info.h>
#include <windows.h>
#include <psapi.h>
#include <process_info.h>

void fetchUserInfo(unsigned long index)
{
    HANDLE processHandle = OpenProcess(
        PROCESS_QUERY_LIMITED_INFORMATION,
        FALSE,
        processList[index].pid);

    if (!processHandle)
    {
        strcpy(processList[index].userName, "N/A");
        strcpy(processList[index].userGroup, "N/A");
        return;
    }

    HANDLE processToken = NULL;

    if (!OpenProcessToken(processHandle, TOKEN_QUERY, &processToken))
    {
        strcpy(processList[index].userName, "N/A");
        strcpy(processList[index].userGroup, "N/A");
        CloseHandle(processHandle);
        return;
    }

    DWORD tokenInfoSize = 0;

    GetTokenInformation(
        processToken,
        TokenUser,
        NULL,
        0,
        &tokenInfoSize);

    if (tokenInfoSize == 0)
    {
        strcpy(processList[index].userName, "N/A");
        strcpy(processList[index].userGroup, "N/A");
        CloseHandle(processToken);
        CloseHandle(processHandle);
        return;
    }

    BYTE* tokenBuffer = new BYTE[tokenInfoSize];

    if (!GetTokenInformation(
            processToken,
            TokenUser,
            tokenBuffer,
            tokenInfoSize,
            &tokenInfoSize))
    {
        strcpy(processList[index].userName, "N/A");
        strcpy(processList[index].userGroup, "N/A");

        delete[] tokenBuffer;
        CloseHandle(processToken);
        CloseHandle(processHandle);
        return;
    }

    TOKEN_USER* tokenUser = (TOKEN_USER*)tokenBuffer;
    PSID userSid = tokenUser->User.Sid;

    char name[256];
    char domain[256];

    DWORD nameSize = sizeof(name);
    DWORD domainSize = sizeof(domain);

    SID_NAME_USE sidType;

    if (!LookupAccountSidA(
            NULL,
            userSid,
            name,
            &nameSize,
            domain,
            &domainSize,
            &sidType))
    {
        strcpy(processList[index].userName, "N/A");
        strcpy(processList[index].userGroup, "N/A");
    }
    else
    {
        strcpy(processList[index].userName, name);
        strcpy(processList[index].userGroup, domain);
    }

    delete[] tokenBuffer;
    CloseHandle(processToken);
    CloseHandle(processHandle);
}
