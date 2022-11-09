#pragma once

#include <Exile/Runtime/API.hpp>

namespace Exi::Runtime
{

    /** Execution environment of a reflection object */
    enum Realm
    {
        /** The object exists in memory */
        Native,

        /** The object exists within a Lua context */
        Lua
    };

}
