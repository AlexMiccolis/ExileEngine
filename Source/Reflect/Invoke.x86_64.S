// On x86_64 the TypedValue struct looks something like this:
//
// struct TypedValue {
//     uint64_t m_Type;        // 0x0
//     void* m_PointerValue;   // 0x8
// };
//

//void* InvokeArgs(RCX Instance, RDX Fn, R8 Args, R9 Count)
.global InvokeArgs
InvokeArgs:
    push %rcx
    push %rsi
    push %rdi
    mov (%rdx), %rax // Save function pointer in RAX
    mov %r9, %rdi
    lea 8(%r8), %rsi

    // Initialize arg registers to null
    xor %rdx, %rdx
    xor %r8, %r8
    xor %r9, %r9

    test %rdi, %rdi
    jz .1f

    mov (%rsi), %rdx // First argument
    add $0x10, %rsi
    dec %rdi
    jz .1f

    mov (%rsi), %r8 // Second argument
    add $0x10, %rsi
    dec %rdi
    jz .1f

    mov (%rsi), %r9 // Third argument
    add $0x10, %rsi
    dec %rdi
    jz .1f

    // Other arguments are on the stack
    // TODO: Deal with stack arguments
.1f:
    pop %rdi
    pop %rsi
    pop %rcx
    jmp *%rax
