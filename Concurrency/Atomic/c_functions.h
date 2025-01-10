using value_t = unsigned long long;

extern "C" value_t asm_load(volatile value_t* buffer_);
extern "C" void asm_store(volatile value_t* buffer_, value_t value_);
extern "C" value_t asm_exchange(volatile value_t* buffer_, value_t value_);
extern "C" value_t asm_fetch_add(volatile value_t* buffer_, value_t arg);
extern "C" value_t asm_fetch_sub(volatile value_t* buffer_, value_t arg);
extern "C" value_t asm_fetch_and(volatile value_t* buffer_, value_t arg);
extern "C" value_t asm_fetch_or(volatile value_t* buffer_, value_t arg);
extern "C" value_t asm_fetch_xor(volatile value_t* buffer_, value_t arg);
