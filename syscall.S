; syscall.S
section .text
global syscall3

syscall3:
    mov rax, rdi    ; syscall number
    mov rdi, rsi    ; first argument
    mov rsi, rdx    ; second argument
    mov rdx, rcx    ; third argument
    syscall
    ret