#pragma once

#include <Exile/TL/UUID.hpp>
#include <Exile/Runtime/API.hpp>
#include <Exile/Runtime/Reflection/Types.hpp>
#include <Exile/Runtime/Reflection/Field.hpp>
#include <Exile/Runtime/Reflection/Method.hpp>
#include <string>
#include <vector>

namespace Exi::Runtime
{

    /** Runtime data for a class */
    class RUNTIME_API Class
    {
    public:
        EXI_NO_COPY(Class);
        Class(std::string name, Realm realm);

        /** Add a method to the class */
        Method& AddMethod(const std::string& name);

        /** Add a field to the class */
        Field& AddField(const std::string& name, Type type);

        [[nodiscard]] const std::string& GetName()     const { return m_Name; }
        [[nodiscard]] const TL::UUID&    GetUniqueId() const { return m_UniqueId; }
        [[nodiscard]] Realm GetRealm() const { return m_Realm; }
        [[nodiscard]] bool  IsNative() const { return m_Realm == Native; }
        [[nodiscard]] bool  IsLua()    const { return m_Realm == Lua; }

        [[nodiscard]] const std::vector<Method>& GetMethods() const { return m_Methods; }
        [[nodiscard]] const std::vector<Field>&  GetFields()  const { return m_Fields; }

        [[nodiscard]] const Method* FindMethod(const std::string& name) const;
        [[nodiscard]] const Field*  FindField(const std::string& name)  const;

    private:
        const std::string m_Name;
        const TL::UUID    m_UniqueId;
        const Realm       m_Realm;

        std::vector<Method> m_Methods;
        std::vector<Field>  m_Fields;
    };

}
