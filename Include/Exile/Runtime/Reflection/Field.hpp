#pragma once

#include <Exile/Runtime/API.hpp>
#include <Exile/Runtime/Reflection/Types.hpp>
#include <string>

namespace Exi::Runtime
{
    class Class;

    class RUNTIME_API Field
    {
    public:
        EXI_NO_COPY(Field);
        Field(const Class& owner, std::string name, Type type);

        [[nodiscard]] const Class& GetOwner() const { return m_Owner; }
        [[nodiscard]] const std::string& GetName() const { return m_Name; }
        [[nodiscard]] Type GetType() const { return m_Type; }

        void SetOffset(std::size_t offset);
    private:
        const Class& m_Owner;
        const std::string m_Name;
        const Type m_Type;

        std::size_t m_Offset = 0;
    };

}
