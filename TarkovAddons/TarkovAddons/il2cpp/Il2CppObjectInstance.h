#pragma once
#include "libil2cpp/il2cpp-class-internals.h"
#include "il2utils.h"

/**
 * Represents an instance of an il2cpp object and provides interfaces for it
 */
class Il2CppObjectInstance
{
public:
    /**
     * Should be used in most cases, automatically sets the class
     * @param instance object instance
     */
    explicit Il2CppObjectInstance(Il2CppObject* instance)
    {
        this->clazz_ = instance->klass;
        this->instance_ = instance;
    }

    /**
     * Explicitly declare the class
     * @param clazz il2cpp class
     * @param instance object instance
     */
    Il2CppObjectInstance(const Il2CppClass* clazz, Il2CppObject* instance)
    {
        this->clazz_ = clazz;
        this->instance_ = instance;
    }

    /**
     * Resolve the class
     * @param image il2cpp image
     * @param namespaze namespce
     * @param name class name
     * @param instance object instance
     */
    Il2CppObjectInstance(const Il2CppImage* image, const char* namespaze, const char* name,
                         Il2CppObject* instance)
    {
        clazz_ = il2utils::resolve_class(image, namespaze, name);
        instance_ = instance;
    }

    template <typename T>
    void set_field(const char* name, T value)
    {
        const FieldInfo* field = il2utils::resolve_field(this->clazz_, name);
        *reinterpret_cast<T*>(reinterpret_cast<UINT64>(instance_) + field->offset) = value; // NOLINT(performance-no-int-to-ptr)
    }

    template <typename T>
    T get_field(const char* name)
    {
        const FieldInfo* field = il2utils::resolve_field(this->clazz_, name);
        return *reinterpret_cast<T*>(reinterpret_cast<UINT64>(instance_) + field->offset); // NOLINT(performance-no-int-to-ptr)
    }

    template <typename T>
    T get_method(const char* name, const int args)
    {
        return il2utils::get_method<T>(this->clazz_, name, args);
    }

    Il2CppObject* get_instance() const
    {
        return instance_;
    }

private:
    const Il2CppClass* clazz_;
    Il2CppObject* instance_;
};
