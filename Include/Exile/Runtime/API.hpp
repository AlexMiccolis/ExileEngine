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

namespace Exi::Runtime
{


}
