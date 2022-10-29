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
    #define RUNTIME_API
#endif

namespace Exi::Runtime
{


}