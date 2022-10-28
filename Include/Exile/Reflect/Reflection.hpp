#pragma once

#include <Exile/Reflect/Compiler.hpp>
#include <cassert>
#include <concepts>
#include <vector>

#include <Exile/TL/Type.hpp>

namespace Exi::Reflect
{
    #pragma region ID Types
    /**
     * Class ID, generated from class names
     * @see Exi::Reflect::Hash
     */
    using ClassId  = TL::TypeId;

    /**
     * Field ID, generated from field names
     * @see Exi::Reflect::Hash
     */
    using FieldId  = TL::TypeId;

    /**
     * Method ID, generated from method names
     * @see Exi::Reflect::Hash
     */
    using MethodId = TL::TypeId;
    #pragma endregion

    /**
     * Forward declaration for ClassBase
     */
    struct alignas(sizeof(void*)) ClassBase;

    /**
     * Static reflection information.
     *
     * Specialization for Base classes.
     */
    template <class Cls, TemplateString ClassName, ClassId ClsId, class SuperClassType>
    struct StaticClass
    {
        /* Declaration type of this class */
        using Class = Cls;

        /* Super type of this class */
        using Super = SuperClassType;

        /* Name of this class type as seen in source code */
        static constexpr const char* Name = ClassName.Data();

        /* ID of this class */
        static constexpr ClassId Id = ClsId;

        /* Type of this class */
        static constexpr TL::Type ClassType = (TL::Type)ClsId;

        /* ID of this class's super class */
        static constexpr ClassId SuperId = SuperClassType::Static::Id;

        /* Whether this class was derived from another class */
        static constexpr bool IsDerived = !std::same_as<Class, SuperClassType>;
    };

    /**
     * Class that all reflection classes are derived from.
     * All classes are aligned to 4 or 8 bytes depending on the native pointer size.
     */
    struct alignas(sizeof(void*)) ClassBase { using Static = StaticClass<ClassBase, "ClassBase", 0, ClassBase>; };

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

    /**
     * TryRegisterClass forward definition
     * @tparam Clazz
     */
    template <ReflectiveClass Clazz>
    static inline void TryRegisterClass();

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
        template <class FieldType, TemplateString Name, ReflectiveClass Owner, std::size_t Offset>
        static inline Field From()
        {
            constexpr FieldId Id = Hash(Name.Data());
            using PtrTraits = PointerTraits<FieldType>;

            /* Don't use TypeValue if FieldType is a Class pointer */
            if constexpr (std::is_pointer_v<FieldType> &&
                    std::derived_from<typename PtrTraits::ValueType, ClassBase>)
            {
                TryRegisterClass<typename PtrTraits::ValueType>();
                return Field(Id, PtrTraits::ValueType::Static::ClassType, Owner::Static::Id, Offset, Name.Data());
            }
            else
            {
                return Field(Id, TL::TypeValueOf<FieldType>::Value, Owner::Static::Id, Offset, Name.Data());
            }
        }

        [[nodiscard]] FieldId GetId() const { return m_Id; }
        [[nodiscard]] TL::Type GetType() const { return m_Type; }
        [[nodiscard]] ClassId GetOwnerId() const { return m_OwnerId; }
        [[nodiscard]] std::size_t GetOffset() const { return m_Offset; }
        [[nodiscard]] const char* GetName() const { return m_Name; }

        TL::TypedValue Get(ClassBase* Instance) const
        {
            TL::TypedValue value(m_Type);
            value.SetValue(&GetBasePointer(Instance)[m_Offset]);
            return value;
        }

        bool Set(ClassBase* Instance, const TL::TypedValue& Value) const
        {
            /* Make sure the types are compatible */
            if (m_Type != Value.GetType())
                return false;

            switch (m_Type)
            {
                case TL::TypeInt8:
                    *reinterpret_cast<int8_t*>(&GetBasePointer(Instance)[m_Offset])  = Value.Get<int8_t>();
                    break;
                case TL::TypeInt16:
                    *reinterpret_cast<int16_t*>(&GetBasePointer(Instance)[m_Offset]) = Value.Get<int16_t>();
                    break;
                case TL::TypeInt32:
                    *reinterpret_cast<int32_t*>(&GetBasePointer(Instance)[m_Offset]) = Value.Get<int32_t>();
                    break;
                case TL::TypeInt64:
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
        friend class Class;

        FieldId m_Id;
        TL::Type m_Type;
        ClassId m_OwnerId;
        std::size_t m_Offset;
        const char* m_Name;

        Field(FieldId Id, TL::Type FieldType, ClassId OwnerId, std::size_t Offset, const char* Name)
            : m_Id(Id), m_Type(FieldType), m_OwnerId(OwnerId), m_Offset(Offset), m_Name(Name) { }

        static inline char* GetBasePointer(ClassBase* Instance)
        {
            return reinterpret_cast<char*>(Instance);
        }

    };

    #include <Exile/Reflect/Impl/Invoke.hpp>

    /**
     * Method data retrievable and usable at runtime.
     */
    class alignas(64) Method
    {
    public:
        template <ReflectiveClass Owner, TemplateString Name, auto Fn>
        static Method From()
        {
            using FnTraits = FunctionTraits<decltype(Fn)>;
            return Method(
                    Hash(Name.Data()),
                    Name.Data(),
                    Owner::Static::Id,
                    RuntimeFunction::From<Fn>()
                    );
        }

        /**
         * Invoke method without checking parameter types or compatibility.
         * Obviously a bit dangerous.
         * @tparam Args
         * @param Instance
         * @param args
         * @return
         */
        template <class... Args>
        [[nodiscard]] void* InvokeUnchecked(ClassBase* Instance, Args&& ...args) const
        {
            return (Instance->*m_Function.GetFunction())(std::forward<Args>(args)...);
        }

        TL::TypedValue Invoke(ClassBase* Instance, TL::TypedValue* Args, std::size_t Count) const
        {
            return m_Function.Invoke(Instance, Args, Count);
        }

        TL::TypedValue Invoke(ClassBase* Instance, std::vector<TL::TypedValue>& Args) const
        {
            return m_Function.Invoke(Instance, Args.data(), Args.size());
        }

        [[nodiscard]] FieldId GetId() const { return m_Id; }
        [[nodiscard]] ClassId GetOwnerId() const { return m_OwnerId; }
        [[nodiscard]] const char* GetName() const { return m_Name; }
        [[nodiscard]] const char* GetSignature() const { return m_Signature; }
    private:
        friend class Class;

        using MethodFn = void* (ClassBase::*)(...);

        MethodId m_Id;
        ClassId m_OwnerId;
        const char* m_Name;
        const char* m_Signature;
        RuntimeFunction m_Function;

        Method(MethodId Id, const char* Name, ClassId OwnerId,
               RuntimeFunction&& Fn)
            : m_Id(Id), m_OwnerId(OwnerId), m_Name(Name), m_Signature(nullptr),
              m_Function(std::move(Fn))
        {

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
            static Class clazz(Clazz::Static::Id,
                               Clazz::Static::SuperId,
                               Clazz::Static::Name);
            return clazz;
        }

        /**
         * Expose a field to reflection
         * @param field
         */
        void Expose(const Field& field)
        {
            m_FieldMap.emplace(field.GetId(), field);
        }

        /**
         * Expose a method to reflection
         * @param method
         */
        void Expose(const Method& method)
        {
            m_MethodMap.emplace(method.GetId(), method);
        }

        /**
         * Get a field by ID
         * @param name
         * @return Pointer to field if it exists, nullptr otherwise
         */
        const Field* GetField(FieldId id) const
        {
            return m_FieldMap.contains(id) ? &m_FieldMap.at(id) : GetInheritedField(id);
        }

        /**
         * Get a field by name
         * @param name
         * @return Pointer to field if it exists, nullptr otherwise
         */
        const Field* GetField(const char* name) const { return GetField(Hash(name)); }

        /**
         * Get a method by ID
         * @param name
         * @return Pointer to method if it exists, nullptr otherwise
         */
        const Method* GetMethod(MethodId id) const
        {
            return m_MethodMap.contains(id) ? &m_MethodMap.at(id) : GetInheritedMethod(id);
        }

        /**
         * Get a method by name
         * @param name
         * @return Pointer to method if it exists, nullptr otherwise
         */
        const Method* GetMethod(const char* name) const { return GetMethod(Hash(name)); }

        /**
         * Retrieve all class fields and write them into an array
         * @param fields Fields array
         * @param maxFields Size of fields array
         * @return Fields count
         */
        std::size_t GetFields(Field const** fields, std::size_t maxFields) const;

        /**
         * Retrieve all class methods and write them into an array
         * @param methods Methods array
         * @param maxMethods Size of methods array
         * @return Method count
         */
        std::size_t GetMethods(Method const** methods, std::size_t maxMethods) const;

        std::size_t GetFieldCount() const { return m_FieldMap.size(); }
        std::size_t GetMethodCount() const { return m_MethodMap.size(); }

        ClassId GetId() const { return m_Id; }
        ClassId GetSuperId() const { return m_SuperId; }
        const char* GetName() const { return m_Name; }
    private:
        Class(ClassId Id, ClassId SuperId, const char* Name)
                : m_Id(Id), m_SuperId(SuperId), m_Name(Name) { }

        const Field* GetInheritedField(FieldId id) const;
        const Method* GetInheritedMethod(MethodId id) const;

        ClassId m_Id;
        ClassId m_SuperId;
        const char* m_Name;
        std::unordered_map<FieldId, Field> m_FieldMap;
        std::unordered_map<MethodId, Method> m_MethodMap;
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
        using Static = Reflect::StaticClass<ClassType, ClassName, ClsId, SuperType>;

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
        using Static = Reflect::StaticClass<ClassType, ClassName, ClsId, ClassBase>;

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
#define ExposeField(ClassRef, Name) \
    constexpr auto Field_##Name##_Offset = offsetof(Self, Name); \
    auto Field_##Name = Exi::Reflect::Field::From<      \
        decltype(Name),          \
        #Name,                   \
        Self,                   \
        Field_##Name##_Offset>();          \
    ClassRef.Expose(Field_##Name);
#define ExposeField_Aliased(ClassRef, Name, Alias) \
    constexpr auto Field_##Alias##_Offset = offsetof(Self, Name); \
    auto Field_##Alias = Exi::Reflect::Field::From<      \
        decltype(Name),          \
        #Alias,                   \
        Self,                   \
        Field_##Alias##_Offset>();          \
    ClassRef.Expose(Field_##Alias);

#define ExposeMethod(ClassRef, Owner, Name) \
    const Exi::Reflect::Method& Method_##Name = Exi::Reflect::Method::From<Owner, #Name, &Owner::Name>(); \
    ClassRef.Expose(Method_##Name);

