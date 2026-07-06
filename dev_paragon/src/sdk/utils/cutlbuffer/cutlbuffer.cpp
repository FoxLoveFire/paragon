#include "cutlbuffer.h"

CUtlBuffer::CUtlBuffer(int a1, int size, int a3)
{
	sdk::ConstructUtlBuffer(this, a1, size, a3);
}

void CUtlBuffer::ensure(int size)
{
	sdk::EnsureCapacityBuffer(this, size);
}

void CUtlBuffer::PutString(const char* szString)
{

	sdk::PutUtlString(this, szString);
}
