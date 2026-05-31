#pragma once

#include <stdint.h>

using TypeIndex = int32_t;
using TypeDefinitionIndex = int32_t;
using FieldIndex = int32_t;
using DefaultValueIndex = int32_t;
using DefaultValueDataIndex = int32_t;
using CustomAttributeIndex = int32_t;
using ParameterIndex = int32_t;
using MethodIndex = int32_t;
using GenericMethodIndex = int32_t;
using PropertyIndex = int32_t;
using EventIndex = int32_t;
using GenericContainerIndex = int32_t;
using GenericParameterIndex = int32_t;
using GenericParameterConstraintIndex = int16_t;
using NestedTypeIndex = int32_t;
using InterfacesIndex = int32_t;
using VTableIndex = int32_t;
using RGCTXIndex = int32_t;
using StringIndex = int32_t;
using StringLiteralIndex = int32_t;
using GenericInstIndex = int32_t;
using ImageIndex = int32_t;
using AssemblyIndex = int32_t;
using InteropDataIndex = int32_t;

// "Relative" indexes - based on their parent
using TypeFieldIndex = int32_t;
using TypeMethodIndex = int32_t;
using MethodParameterIndex = int32_t;
using TypePropertyIndex = int32_t;
using TypeEventIndex = int32_t;
using TypeInterfaceIndex = int32_t;
using TypeNestedTypeIndex = int32_t;
using TypeInterfaceOffsetIndex = int32_t;
using GenericContainerParameterIndex = int32_t;
using AssemblyTypeIndex = int32_t;
using AssemblyExportedTypeIndex = int32_t;

static constexpr TypeIndex kTypeIndexInvalid = -1;
static constexpr TypeDefinitionIndex kTypeDefinitionIndexInvalid = -1;
static constexpr DefaultValueDataIndex kDefaultValueIndexNull = -1;
static constexpr CustomAttributeIndex kCustomAttributeIndexInvalid = -1;
static constexpr EventIndex kEventIndexInvalid = -1;
static constexpr FieldIndex kFieldIndexInvalid = -1;
static constexpr MethodIndex kMethodIndexInvalid = -1;
static constexpr PropertyIndex kPropertyIndexInvalid = -1;
static constexpr GenericContainerIndex kGenericContainerIndexInvalid = -1;
static constexpr GenericParameterIndex kGenericParameterIndexInvalid = -1;
static constexpr RGCTXIndex kRGCTXIndexInvalid = -1;
static constexpr StringLiteralIndex kStringLiteralIndexInvalid = -1;
static constexpr InteropDataIndex kInteropDataIndexInvalid = -1;

#define PUBLIC_KEY_BYTE_LENGTH 8
static constexpr int kPublicKeyByteLength = PUBLIC_KEY_BYTE_LENGTH;

using Il2CppMethodSpec = struct Il2CppMethodSpec
{
    MethodIndex methodDefinitionIndex;
    GenericInstIndex classIndexIndex;
    GenericInstIndex methodIndexIndex;
};

using Il2CppRGCTXDataType = enum Il2CppRGCTXDataType
{
    IL2CPP_RGCTX_DATA_INVALID,
    IL2CPP_RGCTX_DATA_TYPE,
    IL2CPP_RGCTX_DATA_CLASS,
    IL2CPP_RGCTX_DATA_METHOD,
    IL2CPP_RGCTX_DATA_ARRAY,
    IL2CPP_RGCTX_DATA_CONSTRAINED,
};

using Il2CppRGCTXDefinitionData = union Il2CppRGCTXDefinitionData
{
    int32_t rgctxDataDummy;
    MethodIndex __methodIndex;
    TypeIndex __typeIndex;
};

using Il2CppRGCTXConstrainedData = struct Il2CppRGCTXConstrainedData
{
    TypeIndex __typeIndex;
    uint32_t __encodedMethodIndex;
};

using Il2CppRGCTXDefinition = struct Il2CppRGCTXDefinition
{
    Il2CppRGCTXDataType type;
    const void* data;
};

using Il2CppGenericMethodIndices = struct
{
    MethodIndex methodIndex;
    MethodIndex invokerIndex;
    MethodIndex adjustorThunkIndex;
};

using Il2CppGenericMethodFunctionsDefinitions = struct Il2CppGenericMethodFunctionsDefinitions
{
    GenericMethodIndex genericMethodIndex;
    Il2CppGenericMethodIndices indices;
};

static inline uint32_t GetTokenType(uint32_t token)
{
    return token & 0xFF000000;
}

static inline uint32_t GetTokenRowId(uint32_t token)
{
    return token & 0x00FFFFFF;
}

/* Runtime metadata tokens  */
using Il2CppMetadataImageHandle = const struct ___Il2CppMetadataImageHandle*;
using Il2CppMetadataCustomAttributeHandle = const struct ___Il2CppMetadataCustomAttributeHandle*;
using Il2CppMetadataTypeHandle = const struct ___Il2CppMetadataTypeHandle*;
using Il2CppMetadataMethodDefinitionHandle = const struct ___Il2CppMetadataMethodHandle*;
using Il2CppMetadataGenericContainerHandle = const struct ___Il2CppMetadataGenericContainerHandle*;
using Il2CppMetadataGenericParameterHandle = const struct ___Il2CppMetadataGenericParameterHandle*;
