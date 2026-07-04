#include "schema.h"
#include "schema.h"
#include <vector>
#include <algorithm>
#include <Shlobj.h>
#include <shlobj_core.h>


#include "../fnv1a/fnv1a.h"


struct SchemaDumpedData_t
{
	uint32_t hashedName = 0x0ULL;
	std::uint32_t uOffset = 0x0U;
};

static std::vector<SchemaDumpedData_t> dumped_data;

bool Schema::init(const char* ModuleName, int module_type)
{
	schema_system = I::schema_system;
	if (!schema_system)
		return false;

	CSchemaSystemTypeScope* pTypeScope = schema_system->FindTypeScopeForModule(ModuleName);
	if (pTypeScope == nullptr)
		return false;

	CSchemaList<CSchemaClassBinding>* pClassContainer = pTypeScope->GetClassContainer();
	if (!pClassContainer)
		return false;

	const auto& blockContainers = pClassContainer->GetBlockContainers();

	int totalClasses = 0;
	int totalFields = 0;

	for (const auto& container : blockContainers)
	{
		for (auto pBlock = container.GetFirstBlock(); pBlock; pBlock = pBlock->Next())
		{
			CSchemaClassBinding* pClassBinding = pBlock->GetBinding();
			if (!pClassBinding)
				continue;

			const char* className = pClassBinding->m_bindingName();
			SchemaClassFieldDataArray_t* pFields = pClassBinding->m_DataArray();
			const std::uint16_t nFields = pClassBinding->m_DataArraySize();

			if (!className || !pFields || nFields == 0)
				continue;

			totalClasses++;

			for (std::uint16_t f = 0; f < nFields; ++f)
			{
				const char* fieldName = pFields[f].FieldName;
				if (!fieldName)
					continue;

				const std::uint32_t fieldOffset = static_cast<std::uint32_t>(pFields[f].FieldOffset);
				std::string szFieldClassBuffer = std::string(className) + "->" + std::string(fieldName);

				dumped_data.emplace_back(hash_32_fnv1a_const(szFieldClassBuffer.c_str()), fieldOffset);
				totalFields++;
			}
		}
	}

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	printf("[Schema] ");
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	printf("Cached %i fields from %i classes\n", totalFields, totalClasses);

	return true;
}

std::uint32_t SchemaFinder::Get(const uint32_t hashedName)
{
	if (const auto it = std::ranges::find_if(dumped_data, [hashedName](const SchemaDumpedData_t& data)
		{ return data.hashedName == hashedName; });
		it != dumped_data.end())
		return it->uOffset;

	return 0U;
}

