#include <Exile/Runtime/Reflection/Class.hpp>

namespace Exi::Runtime
{

    Class::Class(std::string name, Realm realm)
        : m_Name(std::move(name)), m_Realm(realm)
    {

    }

}
