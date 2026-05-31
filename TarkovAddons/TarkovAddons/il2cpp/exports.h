#pragma once
#include "il2cpp_offsets.h"
#include "libil2cpp/il2cpp-class-internals.h"
#include "libil2cpp/il2cpp-object-internals.h"
#include "libil2cpp/il2cpp-api-types.h"

namespace il2cpp
{
    inline Il2CppClass* il2cpp_class_from_name(
        const Il2CppImage* image, const char* namespaze, const char* name)
    {
        return reinterpret_cast<Il2CppClass*(*)(const Il2CppImage*, const char*, const char*)>(
            il2cpp_offsets::il2cpp_class_from_name)(image, namespaze, name);
    }

    inline Il2CppClass* il2cpp_class_get_nested_types(
        Il2CppClass* klass, void* * iter)
    {
        return reinterpret_cast<Il2CppClass*(*)(Il2CppClass*, void* *)>(
            il2cpp_offsets::il2cpp_class_get_nested_types)(klass, iter);
    }

    inline FieldInfo* il2cpp_class_get_field_from_name(
        Il2CppClass* klass, const char* name)
    {
        return reinterpret_cast<FieldInfo*(*)(Il2CppClass*, const char*)>(
            il2cpp_offsets::il2cpp_class_get_field_from_name)(klass, name);
    }

    inline const MethodInfo* il2cpp_class_get_method_from_name(Il2CppClass* klass, const char* name, int argsCount)
    {
        return reinterpret_cast<MethodInfo*(*)(Il2CppClass*, const char*, int, PVOID)>(
            il2cpp_offsets::il2cpp_class_get_method_from_name)(
            klass, name, argsCount, nullptr);
    }

    inline const Il2CppType* il2cpp_class_get_type(Il2CppClass* klass)
    {
        return reinterpret_cast<Il2CppType*>(reinterpret_cast<UINT64>(klass) + 32); // NOLINT(performance-no-int-to-ptr)
    }

    inline Il2CppDomain* il2cpp_domain_get()
    {
        return reinterpret_cast<Il2CppDomain*(*)()>(il2cpp_offsets::il2cpp_domain_get)();
    }

    inline const Il2CppAssembly** il2cpp_domain_get_assemblies(
        const Il2CppDomain* domain, size_t* size)
    {
        return reinterpret_cast<const Il2CppAssembly**(*)(const Il2CppDomain*, size_t*)>(
            il2cpp_offsets::il2cpp_domain_get_assemblies)(domain, size);
    }

    inline void il2cpp_field_static_get_value(
        FieldInfo* field, void* value)
    {
        return reinterpret_cast<void(*)(FieldInfo*, void*, UINT64)>(
            il2cpp_offsets::il2cpp_field_static_get_value)(field, value, 0);
    }

    inline void il2cpp_field_static_set_value(FieldInfo* field, void* value)
    {
        return reinterpret_cast<void(*)(FieldInfo*, void*)>(
            il2cpp_offsets::il2cpp_field_static_set_value)(field, value);
    }

    inline Il2CppString* il2cpp_string_new(const char* str)
    {
        return reinterpret_cast<Il2CppString*(*)(const char*)>(il2cpp_offsets::il2cpp_string_new)(str);
    }

    inline Il2CppObject* il2cpp_type_get_object(const Il2CppType* type)
    {
        return reinterpret_cast<Il2CppObject*(*)(const Il2CppType*)>(il2cpp_offsets::il2cpp_type_get_object)(type);
    }
}
