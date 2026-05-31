#pragma once

#include <cstdint>

#if !defined(__cplusplus)
#define bool uint8_t
#endif // !__cplusplus

using Il2CppClass = struct Il2CppClass;
using Il2CppType = struct Il2CppType;
using EventInfo = struct EventInfo;
using MethodInfo = struct MethodInfo;
using FieldInfo = struct FieldInfo;
using PropertyInfo = struct PropertyInfo;

using Il2CppAssembly = struct Il2CppAssembly;
using Il2CppArray = struct Il2CppArray;
using Il2CppDelegate = struct Il2CppDelegate;
using Il2CppDomain = struct Il2CppDomain;
using Il2CppImage = struct Il2CppImage;
using Il2CppException = struct Il2CppException;
using Il2CppProfiler = struct Il2CppProfiler;
using Il2CppObject = struct Il2CppObject;
using Il2CppReflectionMethod = struct Il2CppReflectionMethod;
using Il2CppReflectionType = struct Il2CppReflectionType;
using Il2CppString = struct Il2CppString;
using Il2CppThread = struct Il2CppThread;
using Il2CppAsyncResult = struct Il2CppAsyncResult;
using Il2CppManagedMemorySnapshot = struct Il2CppManagedMemorySnapshot;
using Il2CppCustomAttrInfo = struct Il2CppCustomAttrInfo;

using Il2CppProfileFlags = enum
{
    IL2CPP_PROFILE_NONE = 0,
    IL2CPP_PROFILE_APPDOMAIN_EVENTS = 1 << 0,
    IL2CPP_PROFILE_ASSEMBLY_EVENTS = 1 << 1,
    IL2CPP_PROFILE_MODULE_EVENTS = 1 << 2,
    IL2CPP_PROFILE_CLASS_EVENTS = 1 << 3,
    IL2CPP_PROFILE_JIT_COMPILATION = 1 << 4,
    IL2CPP_PROFILE_INLINING = 1 << 5,
    IL2CPP_PROFILE_EXCEPTIONS = 1 << 6,
    IL2CPP_PROFILE_ALLOCATIONS = 1 << 7,
    IL2CPP_PROFILE_GC = 1 << 8,
    IL2CPP_PROFILE_THREADS = 1 << 9,
    IL2CPP_PROFILE_REMOTING = 1 << 10,
    IL2CPP_PROFILE_TRANSITIONS = 1 << 11,
    IL2CPP_PROFILE_ENTER_LEAVE = 1 << 12,
    IL2CPP_PROFILE_COVERAGE = 1 << 13,
    IL2CPP_PROFILE_INS_COVERAGE = 1 << 14,
    IL2CPP_PROFILE_STATISTICAL = 1 << 15,
    IL2CPP_PROFILE_METHOD_EVENTS = 1 << 16,
    IL2CPP_PROFILE_MONITOR_EVENTS = 1 << 17,
    IL2CPP_PROFILE_IOMAP_EVENTS = 1 << 18, /* this should likely be removed, too */
    IL2CPP_PROFILE_GC_MOVES = 1 << 19,
    IL2CPP_PROFILE_FILEIO = 1 << 20
};

using Il2CppProfileFileIOKind = enum
{
    IL2CPP_PROFILE_FILEIO_WRITE = 0,
    IL2CPP_PROFILE_FILEIO_READ
};

using Il2CppGCEvent = enum
{
    IL2CPP_GC_EVENT_START,
    IL2CPP_GC_EVENT_MARK_START,
    IL2CPP_GC_EVENT_MARK_END,
    IL2CPP_GC_EVENT_RECLAIM_START,
    IL2CPP_GC_EVENT_RECLAIM_END,
    IL2CPP_GC_EVENT_END,
    IL2CPP_GC_EVENT_PRE_STOP_WORLD,
    IL2CPP_GC_EVENT_POST_STOP_WORLD,
    IL2CPP_GC_EVENT_PRE_START_WORLD,
    IL2CPP_GC_EVENT_POST_START_WORLD
};

using Il2CppGCMode = enum
{
    IL2CPP_GC_MODE_DISABLED = 0,
    IL2CPP_GC_MODE_ENABLED = 1,
    IL2CPP_GC_MODE_MANUAL = 2
};

using Il2CppStat = enum
{
    IL2CPP_STAT_NEW_OBJECT_COUNT,
    IL2CPP_STAT_INITIALIZED_CLASS_COUNT,
    //IL2CPP_STAT_GENERIC_VTABLE_COUNT,
    //IL2CPP_STAT_USED_CLASS_COUNT,
    IL2CPP_STAT_METHOD_COUNT,
    //IL2CPP_STAT_CLASS_VTABLE_SIZE,
    IL2CPP_STAT_CLASS_STATIC_DATA_SIZE,
    IL2CPP_STAT_GENERIC_INSTANCE_COUNT,
    IL2CPP_STAT_GENERIC_CLASS_COUNT,
    IL2CPP_STAT_INFLATED_METHOD_COUNT,
    IL2CPP_STAT_INFLATED_TYPE_COUNT,
    //IL2CPP_STAT_DELEGATE_CREATIONS,
    //IL2CPP_STAT_MINOR_GC_COUNT,
    //IL2CPP_STAT_MAJOR_GC_COUNT,
    //IL2CPP_STAT_MINOR_GC_TIME_USECS,
    //IL2CPP_STAT_MAJOR_GC_TIME_USECS
};

using Il2CppRuntimeUnhandledExceptionPolicy = enum
{
    IL2CPP_UNHANDLED_POLICY_LEGACY,
    IL2CPP_UNHANDLED_POLICY_CURRENT
};

using Il2CppStackFrameInfo = struct Il2CppStackFrameInfo
{
    const MethodInfo* method;
    uintptr_t raw_ip;
    int sourceCodeLineNumber;
    int ilOffset;
    const char* filePath;
};

using Il2CppMethodPointer = void(*)();

using Il2CppMethodDebugInfo = struct Il2CppMethodDebugInfo
{
    Il2CppMethodPointer methodPointer;
    int32_t code_size;
    const char* file;
};

using Il2CppMemoryCallbacks = struct
{
    void* (*malloc_func)(size_t size);
    void* (*aligned_malloc_func)(size_t size, size_t alignment);
    void (*free_func)(void* ptr);
    void (*aligned_free_func)(void* ptr);
    void* (*calloc_func)(size_t nmemb, size_t size);
    void* (*realloc_func)(void* ptr, size_t size);
    void* (*aligned_realloc_func)(void* ptr, size_t size, size_t alignment);
};

using Il2CppDebuggerTransport = struct
{
    const char* name;
    void (*connect)(const char* address);
    int (*wait_for_attach)(void);
    void (*close1)(void);
    void (*close2)(void);
    int (*send)(void* buf, int len);
    int (*recv)(void* buf, int len);
};

#if !__SNC__ // SNC doesn't like the following define: "warning 1576: predefined meaning of __has_feature discarded"
#ifndef __has_feature // clang specific __has_feature check
#define __has_feature(x) 0 // Compatibility with non-clang compilers.
#endif
#endif

#if _MSC_VER
using Il2CppChar = wchar_t;
#elif __has_feature(cxx_unicode_literals)
typedef char16_t Il2CppChar;
#else
typedef uint16_t Il2CppChar;
#endif

#if _MSC_VER
using Il2CppNativeChar = wchar_t;
#define IL2CPP_NATIVE_STRING(str) L##str
#else
typedef char Il2CppNativeChar;
#define IL2CPP_NATIVE_STRING(str) str
#endif

using il2cpp_register_object_callback = void(*)(Il2CppObject** arr, int size, void* userdata);
using il2cpp_liveness_reallocate_callback = void* (*)(void* ptr, size_t size, void* userdata);
using Il2CppFrameWalkFunc = void(*)(const Il2CppStackFrameInfo* info, void* user_data);
using Il2CppProfileFunc = void(*)(Il2CppProfiler* prof);
using Il2CppProfileMethodFunc = void(*)(Il2CppProfiler* prof, const MethodInfo* method);
using Il2CppProfileAllocFunc = void(*)(Il2CppProfiler* prof, Il2CppObject* obj, Il2CppClass* klass);
using Il2CppProfileGCFunc = void(*)(Il2CppProfiler* prof, Il2CppGCEvent event, int generation);
using Il2CppProfileGCResizeFunc = void(*)(Il2CppProfiler* prof, int64_t new_size);
using Il2CppProfileFileIOFunc = void(*)(Il2CppProfiler* prof, Il2CppProfileFileIOKind kind, int count);
using Il2CppProfileThreadFunc = void(*)(Il2CppProfiler* prof, unsigned long tid);

using Il2CppSetFindPlugInCallback = const Il2CppNativeChar* (*)(const Il2CppNativeChar*);
using Il2CppLogCallback = void(*)(const char*);

using Il2CppBacktraceFunc = size_t(*)(Il2CppMethodPointer* buffer, size_t maxSize);

struct Il2CppManagedMemorySnapshot;

using il2cpp_array_size_t = uintptr_t;
#define ARRAY_LENGTH_AS_INT32(a) ((int32_t)a)

using Il2CppAndroidUpStateFunc = uint8_t(*)(const char* ifName, uint8_t* is_up);
