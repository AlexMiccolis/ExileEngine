#pragma once

#include <cstdint>

#include <Exile/TL/UUID.hpp>

namespace Exi::TL
{
    using TypeId = std::size_t;

    /**
     * Core type definitions, used in language interfaces and serialization
     */
    enum Type : TypeId
    {
        TypeNull    = 0,
        TypeInt8    = 1,
        TypeInt16   = 2,
        TypeInt32   = 3,
        TypeInt64   = 4,
        TypeFloat   = 5,
        TypeDouble  = 6,
        TypeString  = 7,
        TypeBoolean = 8,
        TypeUUID    = 9,


        TypeEND     = TypeUUID,
        TypeObject  = 255
    };

    static constexpr const std::string_view TypeNames[] = {
        "null",
        "int8",
        "int16",
        "int32",
        "int64",
        "float",
        "double",
        "string",
        "boolean",
        "uuid"
    };

    /**
     * Struct to get the real compile-time type from a Type enum value.
     * @tparam T Type enum value
     */
    template <Type T = TypeNull>
    struct RealTypeOf { using Type = std::nullptr_t; };
    template <> struct RealTypeOf<TypeInt8>    { using Type = int8_t;      };
    template <> struct RealTypeOf<TypeInt16>   { using Type = int16_t;     };
    template <> struct RealTypeOf<TypeInt32>   { using Type = int32_t;     };
    template <> struct RealTypeOf<TypeInt64>   { using Type = int64_t;     };
    template <> struct RealTypeOf<TypeFloat>   { using Type = float;       };
    template <> struct RealTypeOf<TypeDouble>  { using Type = double;      };
    template <> struct RealTypeOf<TypeString>  { using Type = const char*; };
    template <> struct RealTypeOf<TypeBoolean> { using Type = bool;        };
    template <> struct RealTypeOf<TypeUUID>    { using Type = TL::UUID;    };

    /**
     * Struct to get the Type enum value from a compile-time type
     * @tparam T
     */
    template <class T>
    struct TypeValueOf { static constexpr Type Value = TypeNull; };
    template <> struct TypeValueOf<int8_t>      { static constexpr Type Value = TypeInt8; };
    template <> struct TypeValueOf<int16_t>     { static constexpr Type Value = TypeInt16; };
    template <> struct TypeValueOf<int32_t>     { static constexpr Type Value = TypeInt32; };
    template <> struct TypeValueOf<int64_t>     { static constexpr Type Value = TypeInt64; };
    template <> struct TypeValueOf<uint8_t>     { static constexpr Type Value = TypeInt8; };
    template <> struct TypeValueOf<uint16_t>    { static constexpr Type Value = TypeInt16; };
    template <> struct TypeValueOf<uint32_t>    { static constexpr Type Value = TypeInt32; };
    template <> struct TypeValueOf<uint64_t>    { static constexpr Type Value = TypeInt64; };
    template <> struct TypeValueOf<float>       { static constexpr Type Value = TypeFloat; };
    template <> struct TypeValueOf<double>      { static constexpr Type Value = TypeDouble; };
    template <> struct TypeValueOf<const char*> { static constexpr Type Value = TypeString; };
    template <> struct TypeValueOf<bool>        { static constexpr Type Value = TypeBoolean; };
    template <> struct TypeValueOf<TL::UUID>    { static constexpr Type Value = TypeUUID; };

    /**
     * Class to store a value corresponding to a Type
     * @see TL::Type
     */
    class TypedValue
    {
    public:
        TypedValue(Type type)
                : m_Type(type) { }

        template <class T>
        TypedValue(Type type, T val)
                : m_Type(type) { Set(val); }

        Type GetType() const { return m_Type; }

        /**
         * Load the value from a void pointer based on the type,
         * sort of dodgy.
         * @param value
         */
        void SetValue(void* value)
        {
            switch (m_Type)
            {
                case TypeNull:
                    m_Value.pointer = nullptr;
                    break;
                case TypeInt8:
                    m_Value.integer = *reinterpret_cast<std::int8_t*>(value);
                    break;
                case TypeInt16:
                    m_Value.integer = *reinterpret_cast<std::int16_t*>(value);
                    break;
                case TypeInt32:
                    m_Value.integer = *reinterpret_cast<std::int32_t*>(value);
                    break;
                case TypeInt64:
                    m_Value.integer = *reinterpret_cast<std::int64_t*>(value);
                    break;
                case TypeFloat:
                    m_Value.float32 = *reinterpret_cast<float*>(value);
                    break;
                case TypeDouble:
                    m_Value.float64 = *reinterpret_cast<double*>(value);
                    break;
                case TypeString:
                    m_Value.string = *reinterpret_cast<const char**>(value);
                    break;
                case TypeBoolean:
                    m_Value.integer = *reinterpret_cast<bool*>(value);
                    break;
                case TypeObject:
                    m_Value.pointer = *reinterpret_cast<void**>(value);
                    break;
            }
        }

        template <class T>
        inline T Get() const;

        template <class T>
        inline void Set(T value);
    private:
        Type m_Type;
        union
        {
            std::size_t integer;
            float       float32;
            double      float64;
            const char* string;
            void*       pointer;
            UUID        uuid;
        } m_Value;
    };

    template <> inline int8_t      TypedValue::Get() const { return m_Value.integer; }
    template <> inline int16_t     TypedValue::Get() const { return m_Value.integer; }
    template <> inline int32_t     TypedValue::Get() const { return m_Value.integer; }
    template <> inline int64_t     TypedValue::Get() const { return m_Value.integer; }
    template <> inline uint8_t     TypedValue::Get() const { return m_Value.integer; }
    template <> inline uint16_t    TypedValue::Get() const { return m_Value.integer; }
    template <> inline uint32_t    TypedValue::Get() const { return m_Value.integer; }
    template <> inline uint64_t    TypedValue::Get() const { return m_Value.integer; }
    template <> inline float       TypedValue::Get() const { return m_Value.float32; }
    template <> inline double      TypedValue::Get() const { return m_Value.float64; }
    template <> inline const char* TypedValue::Get() const { return m_Value.string;  }
    template <> inline void*       TypedValue::Get() const { return m_Value.pointer; }

    template <> inline void TypedValue::Set(std::nullptr_t)    { m_Value.pointer = nullptr; }
    template <> inline void TypedValue::Set(int8_t value)      { m_Value.integer = value; }
    template <> inline void TypedValue::Set(int16_t value)     { m_Value.integer = value; }
    template <> inline void TypedValue::Set(int32_t value)     { m_Value.integer = value; }
    template <> inline void TypedValue::Set(int64_t value)     { m_Value.integer = value; }
    template <> inline void TypedValue::Set(uint8_t value)     { m_Value.integer = value; }
    template <> inline void TypedValue::Set(uint16_t value)    { m_Value.integer = value; }
    template <> inline void TypedValue::Set(uint32_t value)    { m_Value.integer = value; }
    template <> inline void TypedValue::Set(uint64_t value)    { m_Value.integer = value; }
    template <> inline void TypedValue::Set(float value)       { m_Value.float32 = value; }
    template <> inline void TypedValue::Set(double value)      { m_Value.float64 = value; }
    template <> inline void TypedValue::Set(const char* value) { m_Value.string  = value; }
    template <> inline void TypedValue::Set(void* value)       { m_Value.pointer = value; }

}
