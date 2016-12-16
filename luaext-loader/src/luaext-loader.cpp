// luaext_loader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "LuaLinker.h"
#include "Checksum.h"

#include <iostream>

bool Exists(std::wstring filePath);
bool InjectCodeIntoProcess(std::wstring procFileName, PROCESS_INFORMATION *procInfo);

bool InjectFor_v1_5_3656(PROCESS_INFORMATION *procInfo);

int _tmain(int argc, WCHAR* argv[])
{

    std::wstring rootDir = argv[0];
    rootDir = rootDir.substr(0, rootDir.find_last_of(L"\\"));
    std::wstring rootDirFA = rootDir + L"\\" + FORGED_ALLIANCE_EXE;
    std::wstring rootDirSC = rootDir + L"\\" + SUPREME_COMMANDER_EXE;

    std::wcout << L"Running from directory " << rootDir << std::endl;

    std::wstring executableFilePath;

    if (Exists(rootDirFA))
    {
        executableFilePath = rootDirFA;
    }
    else if (Exists(rootDirSC))
    {
        executableFilePath = rootDirSC;
    }
    else
    {
        std::wcout << L"Could not find " << FORGED_ALLIANCE_EXE << L" or " << SUPREME_COMMANDER_EXE << L"." << std::endl;
        std::wcout << L"Make sure you're running this from the same folder as the EXE file." << std::endl;
    }

    // Create the process
    STARTUPINFO         startupInfo = { 0 };
    PROCESS_INFORMATION procInfo = { 0 };
    startupInfo.cb = sizeof(startupInfo);

    bool processCreated = CreateProcess(
        executableFilePath.c_str(),
        L"ForgedAlliance.exe /init \"init_dev.lua\" /EnableDiskWatch /showlog",
        NULL,               // default process security
        NULL,               // default thread security
        TRUE,               // don't inherit handles
        CREATE_SUSPENDED,
        NULL,               // no new environment
        rootDir.c_str(),    // New root directory
        &startupInfo,
        &procInfo) != 0;


    if (!processCreated)
    {
        if (GetLastError() == ERROR_ELEVATION_REQUIRED)
        {
            wprintf(L"ERROR! Vista has decided that launching Supreme Commander requires UAC privilege elevation. There is no good reason for this to happen, but to fix it, right-click on LuaJITer.exe, go to Properties, pick the Compatibility tab, then turn on \"Run this program as an administrator\".\n");
        }
        else
        {
            wprintf(L"ERROR! Couldn't create process for %s (error code: %d).\n", executableFilePath.c_str(), GetLastError());
        }

        return 1;
    }

    bool injectionSucceeded = InjectCodeIntoProcess(executableFilePath, &procInfo);

    if (injectionSucceeded)
    {
        wprintf(L"Code injected. Resuming %s process.\n", executableFilePath.c_str());
        ResumeThread(procInfo.hThread);
    }
    else 
    {
        wprintf(L"Code not injected. Terminating %s.\n", executableFilePath.c_str());
        TerminateProcess(procInfo.hProcess, 0);
    }

    std::cout << "Press ENTER to continue ..";
    std::wcin.get();

    return 0;
}


bool Exists(std::wstring filePath)
{
    bool result = false;
    FILE *file;

    errno_t err = _wfopen_s(&file, filePath.c_str(), L"rb");

    if (err == 0)
    {
        result = true;
        fclose(file);
    }

    return result;
}


bool InjectCodeIntoProcess(std::wstring procFileName, PROCESS_INFORMATION *procInfo)
{
    UINT checksum;

    if (Checksum::GetChecksum(procFileName, &checksum))
    {
        wprintf(L"%s checksum: 0x%08x.\n", procFileName.c_str(), checksum);

        switch (checksum)
        {
        case SUPCOM_1_5_3656: // Supreme Commander Forged Alliance v1.5.3656 official
            return InjectFor_v1_5_3656(procInfo);

        case SUPCOM_1_5_3599: // Supreme Commander Forged Alliance v1.5.3599 official
            return false;

        case SUPCOM_1_1_3280: // Supreme Commander v1.1.3280 official
            return false;

        default: // unknown checksum
            wprintf(L"ERROR! You have an unknown version of Supreme Commander.\n");
            break;
        }
    }
    else 
    {
        wprintf(L"ERROR! Couldn't open %s (error code: %d).\n", procFileName.c_str(), GetLastError());
    }

    return false;
}

bool InjectFor_v1_5_3656(PROCESS_INFORMATION *procInfo)
{
    bool result = false;

    __try 
    {
        HANDLE process = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, procInfo->dwProcessId);

        if (process != NULL)
        {
            int error = 0;

            BYTE hook[1];

            SIZE_T bytesCount;

            UINT hackStartAddr = 0x0090FD81;
            BYTE hackStart[] = { 
                0xE9, 0x7A, 0xB7, 0x2F, 0x00,   // JMP ForgedAl.00C0B500
                0x83, 0xC4, 0x1C,               // ADD ESP,1C
                0xC3                            // RETN
            };

            if (WriteProcessMemory(process, (LPVOID)(hackStartAddr), &hackStart, sizeof(hackStart), &bytesCount) == 0)
            {
                wprintf(L"Could not write hook-code for hack (error code: %d).\n", GetLastError());
                return false;
            }

            FlushInstructionCache(process, NULL, 0);


            ReadProcessMemory(process, (LPVOID)(hackStartAddr), &hook, sizeof(hook), &bytesCount);

            if (hook[0] != 0xE9)
            {
                wprintf(L"Could not write hook-code for hack (error code: %d).\n", GetLastError());
                return false;
            }

            UINT dataAddr = 0x00C0B4B6;
            BYTE data[] = {
                0x6C, 0x75, 0x61, 0x65, 0x78, 0x74, 0x2E, 0x64, 0x6C, 0x6C, 0x00,   // ASCII "luaext.dll"
                0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00,
                0x69, 0x6E, 0x69, 0x74, 0x69, 0x61, 0x6C, 0x69, 0x7A, 0x65, 0x00, 0x00    // ASCII "initialize"
            };

            if (WriteProcessMemory(process, (LPVOID)(dataAddr), &data, sizeof(data), &bytesCount) == 0)
            {
                wprintf(L"Could not write data for hack (error code: %d).\n", GetLastError());
                return false;
            }

            FlushInstructionCache(process, NULL, 0);


            ReadProcessMemory(process, (LPVOID)(dataAddr), &hook, sizeof(hook), &bytesCount);

            if (hook[0] != 0x6C)
            {
                wprintf(L"Could not write hook-code for hack (error code: %d).\n", GetLastError());
                return false;
            }

            UINT hackAddr = 0x00C0B500;
            BYTE hack[] = {
                0x68, 0xB6, 0xB4, 0xC0, 0x00 ,          // PUSH ForgedAl.00C0B4B6
                0xFF, 0x15, 0x88, 0xF4, 0xC0, 0x00,     // CALL DWORD PTR DS : [<&KERNEL32.LoadLibrar>
                0x68, 0xC9, 0xB4, 0xC0, 0x00,           // PUSH ForgedAl.00C0B4C9
                0x50,                                   // PUSH EAX
                0xFF, 0x15, 0x8C, 0xF4, 0xC0, 0x00,           // CALL DWORD PTR DS : [<&KERNEL32.GetProcAdd>
                0x56,                                   // PUSH ESI
                0xFF, 0xD0,                             // CALL EAX
                0x5E,                                   // POP ESI
                0xE9, 0x66, 0x48, 0xD0, 0xFF            // JMP ForgedAl.0090FD86
            };

            if (WriteProcessMemory(process, (LPVOID)(hackAddr), &hack, sizeof(hack), &bytesCount) == 0)
            {
                wprintf(L"Could not write code for hack (error code: %d).\n", GetLastError());
                return false;
            }

            FlushInstructionCache(process, NULL, 0);

            ReadProcessMemory(process, (LPVOID)(hackAddr), &hook, sizeof(hook), &bytesCount);

            if (hook[0] != 0x68)
            {
                wprintf(L"Could not write hook-code for hack (error code: %d).\n", GetLastError());
                return false;
            }

            result = true;
        }
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        wprintf(L"Error: Could inject code into process (error code: %d).", GetLastError());
    }

    return result;
}