#include "ISchemaClass.h"

bool CSchemaType::GetSizes(int* pOutSize, uint8_t* unkPtr) {
	return Memory::call_vfunc<bool>(this, 3U, pOutSize, unkPtr);
}

bool CSchemaType::GetSize(int* out_size) {
	uint8_t smh = 0;
	return GetSizes(out_size, &smh);
}

bool SchemaClassInfoData_t::InheritsFrom(SchemaClassInfoData_t* pClassInfo) {
	if (pClassInfo == this && pClassInfo != nullptr)
		return true;
	else if (pSchemaParent == nullptr || pClassInfo == nullptr)
		return false;

	for (int i = 0; i < nBaseClassesCount; i++) {
		auto& baseClass = pSchemaParent[i];
		if (baseClass.pClass->InheritsFrom(pClassInfo))
			return true;
	}

	return false;
}

void CSchemaSystemTypeScope::FindDeclaredClass(SchemaClassInfoData_t** pReturnClass, const char* szClassName) {
	return Memory::call_vfunc<void>(this, 2U, pReturnClass, szClassName);
}

void GetSchemaClassInfo(uintptr_t address, SchemaClassInfoData_t** pReturn) {
	return Memory::call_vfunc<void>(reinterpret_cast<void*>(address), 38U, pReturn);
}