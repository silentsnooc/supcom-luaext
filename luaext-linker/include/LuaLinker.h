// See copyright notice in scse_copyright.txt

#pragma once

#define SUPREME_COMMANDER_EXE L"SupremeCommander.exe"
#define FORGED_ALLIANCE_EXE L"ForgedAlliance.exe"

// Supreme Commander v1.1.3280 official

#define SUPCOM_1_1_3280 0xFD0F38A1
#define SUPCOM_1_1_3280_HOOKADDR 0x0090A3BD // Not tested

// Supreme Commander Forged Alliance v1.5.3599 official

#define SUPCOM_1_5_3599 0x84B9E832
#define SUPCOM_1_5_3599_HOOKADDR 0x0090A3BD // Not tested

// Supreme Commander Forged Alliance v1.5.3656 official

#define SUPCOM_1_5_3656 0xBB54D47D
#define SUPCOM_1_5_3656_HOOKADDR 0xBB54D47D 

class LuaLinker
{
public:
    static bool Link();
};
