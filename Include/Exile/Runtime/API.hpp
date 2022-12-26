#pragma once

/**
 * Exile Engine Runtime API
 */
#ifdef _WIN32
    #ifdef EXI_BUILDING_RUNTIME
        #define RUNTIME_API __declspec(dllexport)
    #else
        #define RUNTIME_API __declspec(dllimport)
    #endif
#else
    #ifdef __GNUC__
        #define RUNTIME_API __attribute__((visibility("default")))
    #else
        #error Unsupported compiler!
    #endif
#endif

/** Macro to delete copy constructors and define a default noexcept move constructor */
#define EXI_NO_COPY(ClassName) ClassName(const ClassName&) = delete; \
    void operator=(const ClassName&) = delete; \
    ClassName(ClassName&&) noexcept = default;

namespace Exi::Runtime
{


}
