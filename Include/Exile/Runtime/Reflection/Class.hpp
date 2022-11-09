#pragma once

#include <Exile/Runtime/API.hpp>
#include <Exile/Runtime/Reflection/Types.hpp>
#include <string>

namespace Exi::Runtime
{

    /** Runtime data for a class */
    class RUNTIME_API Class
    {
    public:
        Class(std::string name, Realm realm);
        Class(const Class&) = delete;
        void operator=(const Class&) = delete;
        ~Class() = default;

        [[nodiscard]] const std::string& GetName() const { return m_Name; }
        [[nodiscard]] Realm GetRealm() const { return m_Realm; }

    private:
        const std::string m_Name;
        const Realm m_Realm;
    };

}
