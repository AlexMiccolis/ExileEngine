#pragma once

#ifdef __GNUC__
    #define MS_ABI __attribute__((ms_abi))
#else
    #define MS_ABI
#endif


#if defined(__x86_64__) || defined(_M_X64)
    extern "C" MS_ABI void* InvokeArgs(ClassBase* Instance, void* (ClassBase::*Fn)(...), TL::TypedValue* Args, std::size_t Count);
#else
    extern "C" void* InvokeArgs(ClassBase* Instance, void* (ClassBase::*Fn)(...), TypedValue* Args, std::size_t Count);
#endif

/**
 * Reference to a function invocable at runtime
 */
class RuntimeFunction
{
public:
    using FnType = void* (ClassBase::*)(...);
    RuntimeFunction(FnType fn, TL::Type returnType, std::vector<TL::Type>&& argTypes)
            : m_Function(fn), m_ReturnType(returnType), m_ArgTypes(std::move(argTypes)) { }

    template <class Traits, std::size_t... Indices>
    [[nodiscard]] static inline RuntimeFunction From(FnType Fn, std::index_sequence<Indices...>)
    {
        std::vector<TL::Type> types;
        (types.push_back(TL::TypeValueOf<typename Traits::template Arg<Indices>::Type>::Value), ...);
        return RuntimeFunction(Fn, TL::TypeValueOf<typename Traits::Return>::Value, std::move(types));
    }

    /**
     * Create a RuntimeFunction from function Fn
     * @tparam Fn
     * @return RuntimeFunction
     */
    template <auto Fn>
    [[nodiscard]] static inline RuntimeFunction From()
    {
        return From<FunctionTraits<decltype(Fn)>>((FnType)Fn, std::make_index_sequence<
                std::tuple_size_v<typename FunctionTraits<decltype(Fn)>::Arguments>
        >{ });
    }

    FnType GetFunction() const { return m_Function; }
    TL::Type GetReturnType() const { return m_ReturnType; }

    TL::TypedValue Invoke(ClassBase* Instance, TL::TypedValue* Args, std::size_t Count) const
    {
        TL::TypedValue ReturnValue(m_ReturnType, 0);

        /* Wrong amount of arguments */
        if (Count != m_ArgTypes.size())
        {
            fprintf(stderr, "Argument count mismatch\n");
            return { TL::TypeNull, nullptr };
        }

        for (std::size_t i = 0; i < Count; i++)
        {
            /* Type mismatch */
            if (Args[i].GetType() != m_ArgTypes[i])
            {
                fprintf(stderr, "Argument type mismatch\n");
                return { TL::TypeNull, nullptr };
            }
        }

        void* ptr = InvokeArgs(Instance, m_Function, Args, Count);
        ReturnValue.SetValue(&ptr);
        return ReturnValue;
    }

private:
    FnType m_Function;
    TL::Type m_ReturnType;
    std::vector<TL::Type> m_ArgTypes;
};
