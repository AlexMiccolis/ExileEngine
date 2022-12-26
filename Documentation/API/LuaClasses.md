# Lua Classes

Lua classes can be defined using the `Exi.Class` function, which returns an `Exi.Metaclass`
table. The metaclass is used to add methods and members to the class before finalizing it.

```lua
-- Create a metaclass
local class = Exi.Class("MyClass", { x = 1 })

-- Add a method to it
class:Method("Add", function(self, a, b)
    return (a + b) * self.x
end)

-- Finalize it so we can instantiate it later
class:Finalize()
```

```lua
-- Create a new instance of MyClass
local instance = Exi.New("MyClass")

-- Call the method we defined earlier
instance:Add(1, 2)
```