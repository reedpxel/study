.globl change_exec_context
.globl switchContextAsm

switchContextAsm:
    pushq %rbx
    pushq %rbp
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15

    movq %rsp, (%rdi)
    movq (%rsi), %rsp

    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rbp
    popq %rbx

    ret

# the procedure is called with "call" command, it places next rip value on the 
# stack. But rbp and rsp must be changed manually with push

# mov from memory to memory directly is impossible, only through another
# register
# in any asm program push must be called the same amount of times as pop
# push x pushes x on the stack
# pop (x) writes the value on the top of the stack in x and pops the stack
