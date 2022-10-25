#pragma once

#include <Exile/Reflect/Compiler.hpp>
#include <concepts>
#include <vector>

namespace Exi::Reflect
{
    #include <Exile/Reflect/Impl/Type.hpp>

    /**
     * Static reflection information.
     *
     * Specialization for Base classes.
     */
    template <class ClassType, TemplateString ClassName, ClassId ClsId, class SuperClassType = ClassBase>
    struct StaticClass
    {
        /* Declaration type of this class */
        using Type = ClassType;

        /* Super type of this class */
        using SuperType = SuperClassType;

        /* Name of this class type as seen in source code */
        static constexpr const char* Name = ClassName.Data();

        /* ID of this class */
        static constexpr ClassId Id = ClsId;

        /* ID of this class's super class */
        static constexpr ClassId SuperId = 0;

        /* Whether this class was derived from another class */
        static constexpr bool IsDerived = false;
    };

    /**
     * Class that all reflection classes are derived from.
     * All classes are aligned to 4 or 8 bytes depending on the native pointer size.
     */
    struct alignas(sizeof(void*)) ClassBase { };

    #pragma region Class Concepts
    /**
     * Concept to check whether or not the given type is Reflective
     * @tparam Clazz
     */
    template <class Clazz> concept ReflectiveClass = std::derived_from<Clazz, ClassBase>;

    /**
     * Concept to check whether or not a class is a derived class
     * @tparam Clazz
     */
    template <class Clazz> concept DerivedClass = ReflectiveClass<Clazz> && Clazz::IsDerived;
    #pragma endregion

    #pragma region Static Constructors
    template <ReflectiveClass Clazz>
    struct ClassConstructors
    {
        static constexpr void(*Super)(class Class&) = nullptr;
        static constexpr void(*Class)(class Class&) = &Clazz::StaticInitialize;
    };

    template <DerivedClass Clazz>
    struct ClassConstructors<Clazz>
    {
        static constexpr void(*Super)(class Class&) = &Clazz::Super::StaticInitialize;
        static constexpr void(*Class)(class Class&) = &Clazz::StaticInitialize;
    };

    template <class Clazz>
    /**
     * Concept to check if a class has a static constructor (StaticInitialize) function.
     * Static constructors must be public and have the following signature: void (Exi::Reflect::Class&)
     * @tparam Clazz Class
     */
    concept StaticConstructible = requires
    {
        ReflectiveClass<Clazz>;
        std::same_as<decltype(&Clazz::StaticInitialize), void(*)(Class&)>;
        ClassConstructors<Clazz>::Super != ClassConstructors<Clazz>::Class;
    };

    /**
     * Stateless static construction functor
     * @tparam Clazz
     */
    template <class Clazz>
    struct StaticConstructor
    {
        StaticConstructor() = default;
        void operator()() const noexcept { }
    };

    template <StaticConstructible Clazz>
    struct StaticConstructor<Clazz>
    {
        StaticConstructor() = default;
        void operator()(Class& clazz) const noexcept
        {
            using Constructors = ClassConstructors<Clazz>;
            if (Constructors::Super != nullptr)
                StaticConstructor<typename Clazz::Super>()(clazz);
            Constructors::Class(clazz);
        }
    };
    #pragma endregion

    /**
     * Field data retrievable and usable at runtime.
     */
    class alignas(64) Field
    {
    public:
        Field(FieldId Id, Type FieldType, ClassId OwnerId, std::size_t Offset, const char* Name)
                : m_Id(Id), m_Type(FieldType), m_OwnerId(OwnerId), m_Offset(Offset), m_Name(Name) { }

        template <class FieldType, TemplateString Name, ReflectiveClass Owner, std::size_t Offset>
        static Field From()
        {
            return Field(
                    Hash(Name.Data()),
                    TypeValue<FieldType>::Value,
                    Owner::StaticClass::Id,
                    Offset,
                    Name.Data()
                    );
        }

        [[nodiscard]] FieldId GetId() const { return m_Id; }
        [[nodiscard]] Type  GetType() const { return m_Type; }
        [[nodiscard]] ClassId GetOwnerId() const { return m_OwnerId; }
        [[nodiscard]] std::size_t GetOffset() const { return m_Offset; }
        [[nodiscard]] const char* GetName() const { return m_Name; }

        TypedValue Get(ClassBase* Instance) const
        {
            TypedValue value(m_Type);
            value.SetValue(&GetBasePointer(Instance)[m_Offset]);
            return value;
        }

        bool Set(ClassBase* Instance, const TypedValue& Value) const
        {
            /* Make sure the types are compatible */
            if (m_Type != Value.GetType())
                return false;

            switch (m_Type)
            {
                case TypeInt8:
                    *reinterpret_cast<int8_t*>(&GetBasePointer(Instance)[m_Offset]) = Value.Get<int8_t>();
                    break;
                case TypeInt16:
                    *reinterpret_cast<int16_t*>(&GetBasePointer(Instance)[m_Offset]) = Value.Get<int16_t>();
                    break;
                case TypeInt32:
                    *reinterpret_cast<int32_t*>(&GetBasePointer(Instance)[m_Offset]) = Value.Get<int32_t>();
                    break;
                case TypeInt64:
                    *reinterpret_cast<int64_t*>(&GetBasePointer(Instance)[m_Offset]) = Value.Get<int64_t>();
                    break;
                default:
                    return false;
            }

            return true;
        }

        /**
         * Read the field as a value of the given type
         * @tparam T
         * @param Instance
         * @return Value
         */
        template <typename T> requires std::is_trivial_v<T>
        T GetValue(ClassBase* Instance) const
        {
            return *reinterpret_cast<const T*>(&GetBasePointer(Instance)[m_Offset]);
        }

        /**
         * Set the value of the field to a trivial type (Somewhat dangerous)
         * @tparam T
         * @param Instance
         * @return Value
         */
        template <typename T> requires std::is_trivial_v<T>
        void SetValue(ClassBase* Instance, const T& Value) const
        {
            *reinterpret_cast<T*>(&GetBasePointer(Instance)[m_Offset]) = Value;
        }

    private:
        FieldId m_Id;
        Type m_Type;
        ClassId m_OwnerId;
        std::size_t m_Offset;
        const char* m_Name;

        static inline char* GetBasePointer(ClassBase* Instance)
        {
            return reinterpret_cast<char*>(Instance);
        }

    };

    /**
     * Class data retrievable and usable at runtime, one instance exists for each reflective class.
     */
    class alignas(64) Class
    {
    public:
        friend class ClassRegistry;

        template <ReflectiveClass Clazz>
        static Class& FromStaticClass()
        {
            static Class clazz(Clazz::StaticClass::Id,
                               Clazz::StaticClass::SuperId,
                               Clazz::StaticClass::Name);
            return clazz;
        }

        /**
         * Expose a field to reflection
         * @param field
         */
        void ExposeField(const Field& field)
        {
            m_FieldMap.emplace(field.GetId(), field);
        }

        /**
         * Get a field by name
         * @param name
         * @return Pointer to field if it exists, nullptr otherwise
         */
        const Field* GetField(const char* name) const
        {
            auto hash = Hash(name);
            return m_FieldMap.contains(hash) ? &m_FieldMap.at(hash) : nullptr;
        }

        ClassId GetId() const { return m_Id; }
        ClassId GetSuperId() const { return m_SuperId; }
        const char* GetName() const { return m_Name; }
    private:
        Class(ClassId Id, ClassId SuperId, const char* Name)
                : m_Id(Id), m_SuperId(SuperId), m_Name(Name) { }

        ClassId m_Id;
        ClassId m_SuperId;
        const char* m_Name;
        std::unordered_map<FieldId, Field> m_FieldMap;
    };

    /**
     * TryRegisterClass forward definition
     * @tparam Clazz
     */
    template <ReflectiveClass Clazz>
    static inline void TryRegisterClass();

    /**
     * Static reflection information
     */
    template <DerivedClass ClassType, TemplateString ClassName, ClassId ClsId, class SuperClassType>
    struct StaticClass<ClassType, ClassName, ClsId, SuperClassType>
    {
        /* Declaration type of this class */
        using Type = ClassType;

        /* Super type of this class */
        using SuperType = SuperClassType;

        /* Name of this class type as seen in source code */
        static constexpr const char* Name = ClassName.Data();

        /* ID of this class */
        static constexpr ClassId Id = ClsId;

        /* ID of this class's super class */
        static constexpr ClassId SuperId = SuperClassType::StaticClass::Id;

        /* Whether this class was derived from another class */
        static constexpr bool IsDerived = !std::same_as<SuperType, ClassBase>;
    };

    /**
     * Runtime reflection information
     * @tparam ClassType Class declaration type
     * @tparam ClassName Class name
     */
    template <class ClassType, TemplateString ClassName, class SuperType = ClassBase, ClassId ClsId = Hash(ClassName.Data(), 0)>
    struct ReflectionClass : public SuperType
    {
        #pragma region Compile-time Definitions
        using StaticClass = StaticClass<ClassType, ClassName, ClsId, SuperType>;

        using Self  = ClassType;
        using Super = SuperType;
        #pragma endregion

        static inline bool IsRegistered = false;

        ReflectionClass()
        {
            if (!IsRegistered)
            {
                TryRegisterClass<Self>();
                IsRegistered = true;
            }
        };
    };

    template <class ClassType, TemplateString ClassName, ClassId ClsId>
    struct ReflectionClass<ClassType, ClassName, ClassBase, ClsId> : public ClassBase
    {
        #pragma region Compile-time Definitions
        using StaticClass = StaticClass<ClassType, ClassName, ClsId, ClassBase>;

        using Self  = ClassType;
        using Super = ClassBase;
        #pragma endregion

        static inline bool IsRegistered = false;

        ReflectionClass()
        {
            if (!IsRegistered)
            {
                TryRegisterClass<Self>();
            }
        };
    };

    #include <Exile/Reflect/Impl/ClassRegistry.hpp>

    /**
     * Register a class with the Class Registry
     * @tparam Clazz Class to register
     */
    template <ReflectiveClass Clazz>
    static inline void TryRegisterClass()
    {
        ClassRegistry::GetInstance()->RegisterClass<Clazz>();
    }

}

#define DefineClass(Name) class Name : public Exi::Reflect::ReflectionClass<Name, #Name>
#define DeriveClass(Name, Super) class Name : public Exi::Reflect::ReflectionClass<Name, #Name, Super>
#define ExposeField(ClassRef, Owner, Name) \
    constexpr auto Field_##Name##_Offset = offsetof(Owner, Name); \
    auto Field_##Name = Exi::Reflect::Field::From<      \
        decltype(Name),          \
        #Name,                   \
        Owner,                   \
        Field_##Name##_Offset>();          \
    ClassRef.ExposeField(Field_##Name);

