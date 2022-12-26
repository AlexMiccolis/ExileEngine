#pragma once

#include <Exile/Runtime/API.hpp>
#include <Exile/TL/Type.hpp>

namespace Exi::Runtime
{
    using Type = TL::Type;

    /** Execution environment of a reflection object */
    enum Realm
    {
        /** The object exists in memory */
        Native,

        /** The object exists within a Lua context */
        Lua
    };

}
