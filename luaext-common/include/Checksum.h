// See copyright notice in scse_copyright.txt

#pragma once

#include <string>

class Checksum
{
public:
    static bool GetChecksum(std::wstring procFileName, UINT *checksum);
    static UINT Crc32(BYTE *buf, long len);
};
