#pragma once

#pragma region ID Types
/**
 * Class ID, generated from class names
 * @see Exi::Reflect::Hash
 */
using ClassId  = std::size_t;

/**
 * Field ID, generated from field names
 * @see Exi::Reflect::Hash
 */
using FieldId  = std::size_t;

/**
 * Method ID, generated from method names
 * @see Exi::Reflect::Hash
 */
using MethodId = std::size_t;

/**
 * Type ID
 */
using TypeId = std::size_t;
#pragma endregion

/**
 * Base type definitions
 */
enum Type : TypeId
{
    TypeNull = 0,
    TypeInt8,
    TypeInt16,
    TypeInt32,
    TypeInt64,
    TypeFloat,
    TypeDouble,
    TypeString,
    TypeBoolean,
    TypeObject = 32
};

/**
 * Type ID to compile-time type mappings
 */
template <Type T = TypeNull>
struct TypeMap { using Type = std::nullptr_t; };
template <> struct TypeMap<TypeInt8>    { using Type = int8_t;      };
template <> struct TypeMap<TypeInt16>   { using Type = int16_t;     };
template <> struct TypeMap<TypeInt32>   { using Type = int32_t;     };
template <> struct TypeMap<TypeInt64>   { using Type = int64_t;     };
template <> struct TypeMap<TypeFloat>   { using Type = float;       };
template <> struct TypeMap<TypeDouble>  { using Type = double;      };
template <> struct TypeMap<TypeString>  { using Type = const char*; };
template <> struct TypeMap<TypeBoolean> { using Type = bool;        };

/**
 * Forward declaration for ClassBase
 */
struct alignas(sizeof(void*)) ClassBase;

/**
 * Template to get Type ID from parameter type
 * @tparam T
 * @return
 */
template <class T>
struct TypeValue { static constexpr Type Value = TypeNull; };

template <Integer8 T> struct TypeValue<T> { static constexpr Type Value = TypeInt8; };
template <Integer16 T> struct TypeValue<T> { static constexpr Type Value = TypeInt16; };
template <Integer32 T> struct TypeValue<T> { static constexpr Type Value = TypeInt32; };
template <Float32 T> struct TypeValue<T> { static constexpr Type Value = TypeFloat; };
template <Float64 T> struct TypeValue<T> { static constexpr Type Value = TypeDouble; };
template <> struct TypeValue<const char*> { static constexpr Type Value = TypeString; };
template <> struct TypeValue<bool> { static constexpr Type Value = TypeBoolean; };

/**
 * Class to contain field values and method parameters
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
                m_Value.object = nullptr;
                break;
            case TypeInt8:
                m_Value.integer = *reinterpret_cast<const std::int8_t*>(value);
                break;
            case TypeInt16:
                m_Value.integer = *reinterpret_cast<const std::int16_t*>(value);
                break;
            case TypeInt32:
                m_Value.integer = *reinterpret_cast<const std::int32_t*>(value);
                break;
            case TypeInt64:
                m_Value.integer = *reinterpret_cast<const std::int64_t*>(value);
                break;
            case TypeFloat:
                m_Value.float32 = *reinterpret_cast<const float*>(value);
                break;
            case TypeDouble:
                m_Value.float64 = *reinterpret_cast<const double*>(value);
                break;
            case TypeString:
                m_Value.string = *reinterpret_cast<const char**>(value);
                break;
            case TypeObject:
                m_Value.object = *reinterpret_cast<ClassBase**>(value);
                break;
        }
    }

    void* Get() const
    {
        return m_Value.object;
    }

    template <class T>
    inline T Get() const { static_assert("TypedValue::Get has no specialization for type"); }

    template <class T>
    inline void Set(T value) { static_assert("TypedValue::Set has no specialization for type"); }
private:
    Type m_Type;
    union
    {
        void* m_PointerValue;
        union
        {
            std::size_t integer;
            float float32;
            double float64;
            const char* string;
            ClassBase* object;
        } m_Value;
    };
};

template <> inline int8_t  TypedValue::Get() const { return m_Value.integer; };
template <> inline int16_t TypedValue::Get() const { return m_Value.integer; };
template <> inline int32_t TypedValue::Get() const { return m_Value.integer; };
template <> inline int64_t TypedValue::Get() const { return m_Value.integer; };
template <> inline size_t  TypedValue::Get() const { return m_Value.integer; };
template <> inline float   TypedValue::Get() const { return m_Value.float32; }
template <> inline double  TypedValue::Get() const { return m_Value.float64; }
template <> inline const char*      TypedValue::Get() const { return m_Value.string; }
template <> inline const ClassBase* TypedValue::Get() const { return m_Value.object; }

template <> inline void TypedValue::Set(const int8_t value)  { m_Value.integer = value; }
template <> inline void TypedValue::Set(const int16_t value) { m_Value.integer = value; }
template <> inline void TypedValue::Set(const int32_t value) { m_Value.integer = value; }
template <> inline void TypedValue::Set(const int64_t value) { m_Value.integer = value; }
template <> inline void TypedValue::Set(const size_t value)  { m_Value.integer = value; }
template <> inline void TypedValue::Set(const float& value)  { m_Value.float32 = value; }
template <> inline void TypedValue::Set(const double& value) { m_Value.float64 = value; }
template <> inline void TypedValue::Set(const char* value)   { m_Value.string  = value; }
template <> inline void TypedValue::Set(ClassBase* value) { m_Value.object  = value; }
