//void* InvokeArgs(RCX Instance, RDX Fn, R8 Args, R9 Count)
.global InvokeArgs
InvokeArgs:
    push %rcx
    push %rsi
    push %rdi
    mov (%rdx), %rax
    mov %r9, %rdi
    mov %r8, %rsi

    // Initialize arg registers to null
    xor %rdx, %rdx
    xor %r8, %r8
    xor %r9, %r9

    test %rdi, %rdi
    jz .1f

    mov (%rsi), %rdx // First argument
    dec %rdi
    jz .1f

    mov 8(%rsi), %r8 // Second argument
    dec %rdi
    jz .1f

    mov 16(%rsi), %r9 // Third argument
    dec %rdi
    jz .1f

    // Other arguments are on the stack
    // TODO: Deal with stack arguments
.1f:
    pop %rdi
    pop %rsi
    pop %rcx
    jmp *%rax
