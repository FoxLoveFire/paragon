#pragma once
#include "keyvalues.h"
#include <cstdint>
#include "../cutlbuffer/cutlbuffer.h"
#include "../../sdk.h"

#include <iostream>

void CKeyValues3::LoadFromBuffer(const char* szString)
{
    CUtlBuffer buffer(0, (std::strlen(szString) + 10), 1);

    buffer.PutString(szString);
    this->LoadKV3(&buffer);
}

bool CKeyValues3::LoadKV3(CUtlBuffer* buffer)
{
    KV3ID_t kv3ID = KV3ID_t("generic", 0x41B818518343427E, 0xB5F447C23C0CDF8C);
    return sdk::LoadKeyValues(this, nullptr, buffer, &kv3ID, nullptr);
}

CKeyValues3* CKeyValues3::create_material_from_resource()
{
    using fnSetTypeKV3 = CKeyValues3 * (__fastcall*)(CKeyValues3*, unsigned int, unsigned int);

    fnSetTypeKV3 oSetTypeKV3 = reinterpret_cast<fnSetTypeKV3>(
        Memory::pattern_scan("40 53 48 83 EC ? 4C 8B 11 41 B9", "tier0.dll")
        );

    if (!oSetTypeKV3) {
        printf("[!] Failed to find oSetTypeKV3 pattern!\n");
        return nullptr;
    }

    CKeyValues3* pKeyValue = new CKeyValues3[0x10];
    return oSetTypeKV3(pKeyValue, 1U, 6U);
}
