#pragma once
#include "../../../utils/schema/schema.h"
#include "../../../memory/memory.h"
#include "../../../utils/CHandle/handle.h"

class CEntityInstance;
class CEntityIdentity
{
public:
	SCHEMA_ADD_OFFSET(std::uint32_t, index, 0x10);
	schema(const char*, m_designerName, "CEntityIdentity->m_designerName");
	schema(std::uint32_t, flags, "CEntityIdentity->m_flags");

	std::string GetSchemaName() {

		auto class_info = *(uintptr_t*)(std::uintptr_t(this) + 0x10);
		auto name = *(const char*)(std::uintptr_t(this) + 0x20);

		return std::to_string(name);
	}

	[[nodiscard]] bool valid()
	{
		return index() != INVALID_EHANDLE_INDEX;
	}

	[[nodiscard]] int get_index()
	{
		if (!valid())
			return ENT_ENTRY_MASK;

		return index() & ENT_ENTRY_MASK;
	}

	[[nodiscard]] int get_serial_number()
	{
		return index() >> NUM_SERIAL_NUM_SHIFT_BITS;
	}

	CEntityInstance* pInstance;
};

class CEntityInstance
{
public:

	schema(CEntityIdentity*, m_pEntityIdentity, "CEntityInstance->m_pEntity");

	SchemaClassInfoData_t* get_schema_class_info()
	{
		if (!this || (uintptr_t)this <= 0x1000) return nullptr;
		constexpr unsigned int kGetSchemaClassInfoIndex = 46U;
		SchemaClassInfoData_t* ci = nullptr;
		Memory::call_vfunc<void>(this, kGetSchemaClassInfoIndex, &ci);
		return ci;
	}

	const char* get_class_name()
	{
		SchemaClassInfoData_t* ci = get_schema_class_info();
		return ci ? ci->get_name() : nullptr;
	}


	[[nodiscard]] std::uint32_t get_entity_by_handle()
	{
		CEntityIdentity* identity = m_pEntityIdentity();
		if (identity == nullptr)
			return 0;


		return identity->get_index();
	}

	[[nodiscard]] CBaseHandle handle()
	{
		CEntityIdentity* identity = m_pEntityIdentity();
		if (identity == nullptr)
			return CBaseHandle();

		return CBaseHandle(identity->get_index(), identity->get_serial_number() - (identity->flags() & 1));
	}

};
