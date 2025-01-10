.globl asm_load
.globl asm_store
.globl asm_exchange
.globl asm_fetch_add
.globl asm_fetch_sub
.globl asm_fetch_and
.globl asm_fetch_or
.globl asm_fetch_xor

asm_load: 
    movq (%rdi), %rax // mov does not interrupt atomicity, because each core has
    // its own %rax
    ret

asm_store: // я скопировал это с godbolt.org с флагом memory_order_seq_cst, без
// понятия зачем что-то записывать в %rax, если функция ничего не возвращает.
// Без понятия также, как этот код опеспечивает гарантии флага. Я нагуглил что
// делают флаги (relaxed гарантирует только атомарность, операции чтения и 
// записи процессор и компилятор могут свапать местами для оптимизации,
// acquire/release не дают операциям работы с памятью выходить за пределы границ
// acquire и release, sequently_consistent заставляет вообще все операции чтения 
// и записи в потоке выполняться ровно в том порядке, в каком они написаны в 
// коде на С++), но нигде в реализациях атомике нет ассемблерных команд "Не 
// двигай операции обращения к памяти до/после этой операции", такой операции 
// нет. Тогда какая операция это делает? Mov? В документации нет ничего о таком
// свойстве mov. Короче, этой информации в интернете просто нет, это можно 
// узнать только при личном общении с человеком, который это знает.
// С другими флагами реализация такая:
//    movq %rsi, (%rdi)
//    ret
// Чем одна реализация лучше другой, совершенно неясно.
// Еще вопрос: почему методы load(), store() и exchange() с флагами relaxed и
// acquire/release (acquire для load, release для store) в ассемблерном коде
// (проверка на godbolt) абсолютно одинаковы?
    movq %rsi, %rax
    xchgq (%rdi), %rax // xchg is atomic even if "lock" prefix is not written
    ret

asm_exchange:
    movq %rsi, %rax
    xchgq (%rdi), %rax
    ret

asm_fetch_add:
    lock addq %rsi, (%rdi)
    movq (%rdi), %rax
    ret

asm_fetch_sub:
    lock subq %rsi, (%rdi)
    movq (%rdi), %rax
    ret

asm_fetch_and:
    lock andq %rsi, (%rdi)
    movq (%rdi), %rax
    ret

asm_fetch_or:
    lock orq %rsi, (%rdi)
    movq (%rdi), %rax
    ret

asm_fetch_xor:
    lock xorq %rsi, (%rdi)
    movq (%rdi), %rax
    ret