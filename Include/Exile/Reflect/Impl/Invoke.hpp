#pragma once

#ifdef __GNUC__
    #define MS_ABI __attribute__((ms_abi))
#else
    #define MS_ABI
#endif


#if defined(__x86_64__) || defined(_M_X64)
    extern "C" MS_ABI void* InvokeArgs(ClassBase* Instance, void* (ClassBase::*Fn)(...), void** Args, std::size_t Count);
#else
    extern "C" void* InvokeArgs(ClassBase* Instance, void* (ClassBase::*Fn)(...), void** Args, std::size_t Count);
#endif
