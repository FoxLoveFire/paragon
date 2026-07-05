#pragma once

#include <array>
#include <cstdint>
#include <cstddef>
#include "../../../../memory/memory.h"

#define SCHEMA_SYSTEM_INTERFACE_VERSION "SchemaSystem_001"

namespace GeneratorOffset
{
    constexpr std::ptrdiff_t GetNumSchema = 0x74;
    constexpr std::ptrdiff_t GetClassContainer = 0x5C0;
}

using SchemaString_t = const char*;

struct SchemaMetadataEntryData_t;
class CSchemaType;
class CSchemaSystemTypeScope;
struct CSchemaClassBinding;

struct SchemaClassFieldData_t
{
    SchemaString_t szName;
    CSchemaType* pSchemaType;
    std::uint32_t nSingleInheritanceOffset;
    std::int32_t nMetadataSize;
    SchemaMetadataEntryData_t* pMetaData;
};

class SchemaClassFieldDataArray_t
{
public:
    const char* FieldName;
    CSchemaType* FieldType;
    int FieldOffset;

private:
    std::byte pad0[0xC];
};

struct SchemaClassInfoData_t;
struct SchemaStaticFieldData_t;

struct SchemaBaseClassInfoData_t
{
    unsigned int uOffset;
    SchemaClassInfoData_t* pClass;
};

struct SchemaClassInfoData_t
{
    std::byte pad01[0x8];
    const char* szName;
    char* szModuleName;
    int m_nSize;
    std::int16_t nFieldSize;
    std::int16_t nStaticSize;
    std::int16_t nMetadataSize;
    std::uint8_t nAlignOf;
    std::uint8_t nBaseClassesCount;
    std::byte pad04[0x4];

    SchemaClassFieldData_t* pFields;
    SchemaStaticFieldData_t* pStaticFields;
    SchemaBaseClassInfoData_t* pSchemaParent;

    bool InheritsFrom(SchemaClassInfoData_t* pClassInfo);
    const char* get_name() const { return szName; }
};

class CSchemaType
{
public:
    bool GetSizes(int* pOutSize, uint8_t* unkPtr);
    bool GetSize(int* pOutSize);

public:
    void* pVtable;
    const char* szName;
    CSchemaSystemTypeScope* pSystemTypeScope;
    uint8_t nTypeCategory;
    uint8_t nAatomicCategory;
};

struct CSchemaClassBinding
{
    class CBaseClass_t
    {
    public:
        CSchemaClassBinding* m_classInfo() const
        {
            return *reinterpret_cast<CSchemaClassBinding* const*>(reinterpret_cast<const std::uint8_t*>(this) + 0x8);
        }
    };

    const char* m_bindingName() const
    {
        return *reinterpret_cast<const char* const*>(reinterpret_cast<const std::uint8_t*>(this) + 0x8);
    }

    const char* m_dllName() const
    {
        return *reinterpret_cast<const char* const*>(reinterpret_cast<const std::uint8_t*>(this) + 0x10);
    }

    int m_SizeOf() const
    {
        return *reinterpret_cast<const int*>(reinterpret_cast<const std::uint8_t*>(this) + 0x18);
    }

    std::uint16_t m_DataArraySize() const
    {
        return *reinterpret_cast<const std::uint16_t*>(reinterpret_cast<const std::uint8_t*>(this) + 0x24);
    }

    SchemaClassFieldDataArray_t* m_DataArray() const
    {
        return *reinterpret_cast<SchemaClassFieldDataArray_t* const*>(reinterpret_cast<const std::uint8_t*>(this) + 0x30);
    }

    CBaseClass_t* m_baseClass() const
    {
        return *reinterpret_cast<CBaseClass_t* const*>(reinterpret_cast<const std::uint8_t*>(this) + 0x30);
    }

    CSchemaSystemTypeScope* m_TypeScope() const
    {
        return *reinterpret_cast<CSchemaSystemTypeScope* const*>(reinterpret_cast<const std::uint8_t*>(this) + 0x48);
    }

    CSchemaType* m_Type() const
    {
        return *reinterpret_cast<CSchemaType* const*>(reinterpret_cast<const std::uint8_t*>(this) + 0x50);
    }
};

template <class T = CSchemaClassBinding>
class CSchemaList
{
public:
    class SchemaBlock
    {
    public:
        SchemaBlock* Next() const { return m_nextBlock; }
        T* GetBinding() const { return m_classBinding; }
    private:
        void* m_unk0;
        SchemaBlock* m_nextBlock;
        T* m_classBinding;
    };

    class BlockContainer
    {
    public:
        SchemaBlock* GetFirstBlock() const { return m_firstBlock; }
    private:
        void* m_unk[2];
        SchemaBlock* m_firstBlock;
    };

    using BlockContainers = std::array<BlockContainer, 256>;

    int GetNumSchema() const
    {
        return *reinterpret_cast<const int*>(reinterpret_cast<const std::uint8_t*>(this) - GeneratorOffset::GetNumSchema);
    }

    const BlockContainers& GetBlockContainers() const
    {
        return *reinterpret_cast<const BlockContainers*>(this);
    }
};

class CSchemaSystemTypeScope
{
public:
    void FindDeclaredClass(SchemaClassInfoData_t** pReturnClass, const char* szClassName);

    CSchemaList<CSchemaClassBinding>* GetClassContainer()
    {
        return reinterpret_cast<CSchemaList<CSchemaClassBinding>*>(reinterpret_cast<std::uint8_t*>(this) + GeneratorOffset::GetClassContainer);
    }

    const CSchemaList<CSchemaClassBinding>* GetClassContainer() const
    {
        return reinterpret_cast<const CSchemaList<CSchemaClassBinding>*>(reinterpret_cast<const std::uint8_t*>(this) + GeneratorOffset::GetClassContainer);
    }
};

class ISchemaSystem
{
public:
    CSchemaSystemTypeScope* GlobalTypeScope()
    {
        return Memory::call_vfunc<CSchemaSystemTypeScope*>(this, 11);
    }

    CSchemaSystemTypeScope* FindTypeScopeForModule(const char* szModuleName)
    {
        return Memory::call_vfunc<CSchemaSystemTypeScope*>(this, 13, szModuleName, nullptr);
    }
};

void GetSchemaClassInfo(uintptr_t address, SchemaClassInfoData_t** pReturn);