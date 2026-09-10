#pragma once

struct CBufferString {
    int m_nLength;
    int m_nAllocatedSize;
    union {
        char* m_pString;
        char m_szString[8];
    };

    CBufferString() : m_nLength(0), m_nAllocatedSize(0x80000000 | 0x40000000 | 8) {
        m_pString = nullptr;
    }

    static const char* (__fastcall* fnInsert)(void* ecx, int, const char*, int, bool);

    void Insert(int index, const char* str, int length, bool b1) {
        if (fnInsert) fnInsert(this, index, str, length, b1);
    }
};

inline const char* (__fastcall* CBufferString::fnInsert)(void*, int, const char*, int, bool) = nullptr;