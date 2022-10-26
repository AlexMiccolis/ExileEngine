#pragma once

/**
 * Class Registry
 */
class ClassRegistry
{
public:
    static ClassRegistry* GetInstance();

    /**
     * Register a class and run its static constructor if necessary
     * @tparam Clazz Class to register
     * @return
     */
    template <ReflectiveClass Clazz>
    void RegisterClass()
    {
        constexpr size_t id = Clazz::Static::Id;
        if (IsClassRegistered(id))
            return;

        auto clazz = Class::FromStaticClass<Clazz>();
        if constexpr (StaticConstructible<Clazz>)
            Clazz::StaticInitialize(RegisterClass(clazz, id));
        else
            RegisterClass(clazz, id);

        Clazz::IsRegistered = true;
    }

    /**
     * Register the provided class
     * @param clazz Class information
     * @param id Class ID
     * @return A mutable reference to the class in the class map
     */
    Class& RegisterClass(const Class& clazz, ClassId id);

    /**
     *
     * @param id Class ID
     * @return Whether or not the class is registered
     */
    bool IsClassRegistered(ClassId id) const;

    /**
     * Get a reference to reflection information about a class,
     * registers the class if it isn't already registered
     * @tparam Clazz
     * @return Pointer to class information
     */
    template <ReflectiveClass Clazz>
    const Class* GetClass()
    {
        if (!Clazz::IsRegistered)
            RegisterClass<Clazz>();
        return GetClass(Clazz::Static::Id);
    }

    /**
     * Get a reference to reflection information about a class
     * @param id Class ID
     * @return Pointer to class information if it exists, nullptr otherwise
     */
    const Class* GetClass(ClassId id) const;
private:
    static ClassRegistry* s_Instance;

    ClassRegistry();
    ~ClassRegistry();

    ClassRegistry(ClassRegistry&) = delete;
    ClassRegistry& operator=(const ClassRegistry&) = delete;

    std::unordered_map<ClassId, Class> m_ClassMap;
};