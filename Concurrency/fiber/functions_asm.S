.globl change_exec_context
.globl switchContextAsm
.globl switchStackAsm

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

switchStackAsm:
    movq (%rsp), %rax # writing return address to %rax
    
    pushq %rbx
    pushq %rbp
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15
   
    movq %rsp, (%rdi)
    movq (%rsi), %rsp
    pushq %rax # placing return address on new stack

    ret

#change_exec_context:
# the procedure is called with "call" command, it places previous rbp and 
# next rip value on the stack. But rbp and rsp must be changed manually with
# push

# mov from memory to memory directly is impossible, only through another
# register
# in any asm program push must be called the same amount of times as pop
# push x pushes x on the stack
# pop (x) writes the value on the top of the stack in x and pops the stack
#    movq 16(%rbp), %rdx
#    movq %rdx, (%rbp)
#    movq 24(%rbp), %rdx
#    addq $20, %rdx
#    movq %rdx, 8(%rbp)
#    ret
