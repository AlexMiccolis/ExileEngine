# Exi::Reflect - Reflection System
<p style="border-radius: 3px; border-bottom: 4px solid gray"></p>

## <p style="border-radius: 2px; border-bottom: 2px solid gray">Notable Files</p>
+ Reflection.hpp
  + Main file for including the reflection subsystem
  + Ties together all implementation details of Exi::Reflect
+ Compiler.hpp
  + Compile-time utilities and templates

## <p style="border-radius: 2px; border-bottom: 2px solid gray">Usage</p>
### <p style="border-radius: 2px; border-bottom: 2px solid gray">Classes</p>
Reflective Classes are defined using the `DefineClass` and `DeriveClass`
macros that can are included with `<Exile/Reflect/Reflection.hpp>`.

This is the simplest example of a reflective class:
```c++
DefineClass(MyClass)
{
public:
    MyClass() : m_MyInt(1) { }
private:
    int m_MyInt;
}
```

Under the hood, `MyClass` will extend `ReflectionClass` and pass it
template parameters such as the class itself and its name as a
`TemplateString`. Unfortunately, the `DefineClass` macro is necessary
for collecting the name of the class and passing it to the `ReflectionClass`
template.

If we wanted to derive another class from `MyClass`, it would be done like this:

```c++
DeriveClass(MyDerivedClass, MyClass)
{
public:
    MyDerivedClass() : m_MyOtherInt(2) { }
private:
    int m_MyOtherInt;
}
```

The `DeriveClass` macro will include `MyClass` into the inheritance chain while
providing its compile-time type information to the `ReflectionClass` template.
This is necessary for obtaining the superclass information, which can be accessed
by using the `Super` type inside reflective classes.

The `ReflectionClass` template provides useful compile-time type information, but it
also keeps track of the registration state of the class. The following type definitions
are provided by `ReflectionClass`:

```c++
// Static compile-time type information
using StaticClass = StaticClass<ClassType, ClassName, ClsId, SuperType>;

// The type of the current class
using Self  = ClassType;

// The type of the parent class, if any
using Super = SuperType;
```

And within `StaticClass` is:

```c++
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
```

### <p style="border-radius: 2px; border-bottom: 2px solid gray">Class Registration</p>

Classes are registered in the `ClassRegistry` upon the first instance being created. 
The `ReflectionClass` constructor checks the registration state and if the class is
not registered, it calls `ClassRegistry::RegisterClass<>`. Class registration creates
a single constant `Class` instance within the registry for each reflective class, which
contains runtime information such as exposed fields.

### <p style="border-radius: 2px; border-bottom: 2px solid gray">Static Constructors</p>

Static constructors are a way to take control over how the reflection system
sees a class. They provide you with a mutable reference to the `ClassRegistry`'s 
internal `Class` for your object. This allows you to expose internal fields to
the reflection system using the `ExposeField` macro like so:

```c++
DeriveClass(MyDerivedClass, MyClass)
{
public:
    MyDerivedClass() : m_MyOtherInt(2) { }
    
    // The StaticInitialize function MUST be static AND public 
    // or else it won't be detected!
    static void StaticInitialize(Exi::Reflect::Class& Class)
    {
        // This macro creates variables, so it's important
        // to be careful where you put it
        ExposeField(Class, MyDerivedClass, m_MyOtherInt);
    }
private:
    int m_MyOtherInt;
}
```

If the `StaticInitialize` function is detected, it will be called by the 
`ClassRegistry` when the first instance of the object is created.
Static constructors are called in the same order as regular constructors,
that is, `BaseClass::StaticInitialize` will be called before 
`DerivedClass::StaticInitialize`.

### <p style="border-radius: 2px; border-bottom: 2px solid gray">Fields</p>

Field access is arguably the of the most important job of a reflection system, and
it can be done (relatively) easily with the `Field` class.

```c++
void SetOtherInt(MyDerivedClass* Instance, int OtherInt)
{
    // Get the ClassRegistry instance
    auto Registry = Exi::Reflect::ClassRegistry::GetInstance();
    
    // Get a pointer to the MyDerivedClass reflection information
    auto Class = Registry->GetClass<MyDerivedClass>();
    
    // Finally, get a pointer to the m_MyOtherInt field
    auto Field = Class->GetField("m_MyOtherInt");
    
    // Create a TypedValue which can be written to a field
    Exi::Reflect::TypedValue Value(Exi::Reflect::TypeInt32, OtherInt);
    
    // Write the value to the instance's m_MyOtherInt field
    Field->Set(&Instance, SetVal);
}

int GetOtherInt(MyDerivedClass* Instance)
{
    auto Registry = Exi::Reflect::ClassRegistry::GetInstance();
    auto Class = Registry->GetClass<MyDerivedClass>();
    auto Field = Class->GetField("m_MyOtherInt");
    
    // Get a TypedValue from the Field
    auto Value = Field->Get(&Instance);
    
    // Read the TypedValue as an int
    return Value.Get<int>();
}
```

Using these helper functions, we can actually start changing values.

```c++

// If this is the first instance, StaticInitialize is called
// and the fields are exposed and added to the internal field map
MyDerivedClass Derived;

GetOtherInt(&Derived); // Returns 2
SetOtherInt(&Derived, 1337);
GetOtherInt(&Derived); // Returns 1337
```

## <p style="border-radius: 2px; border-bottom: 2px solid gray">Performance</p>
Performance should always be a concern, but especially in a game engine. Many
of the repetitive calculations such a string comparison/hashing are done at
compile time for this reason, but there's unfortunately no such thing as a 
zero-cost abstraction. 

The general performance rule for this reflection system is that the 
first instance of a class will be the slowest to construct.
If you're acquiring a lock and need to be in and out quickly, consider
making the first instance beforehand and getting it out of the way.

Of course, this rule is only relevant if you're using reflective classes for
performance-critical objects. The performance penalty of registering a class
is in the order of 100 nanoseconds, but it depends more on the complexity of 
the static constructor.

### <p style="border-radius: 2px; border-bottom: 2px solid gray">Registration Checking</p>
Every time a reflection class constructor is called, it checks a static
boolean to see if it needs to be registered. 

### <p style="border-radius: 2px; border-bottom: 2px solid gray">Static Constructors</p>
Static constructors are only run once when the class isn't registered, so the
only performance cost will be the cost of the static constructor function 
itself.
