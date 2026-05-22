#ifndef EXECUTION_CONTEXT_H
#define EXECUTION_CONTEXT_H

#include <cstdint>

class ExecutionContext
{
public:
    ExecutionContext(void* rsp = nullptr);
    void switchTo(ExecutionContext& other);
    void setRsp(void* rsp);
    void* getRsp();
public:
    static const uint64_t CALLEE_SAVED_REGISTERS = 7;
    static const uint64_t CALLEE_SAVED_REGISTERS_ON_STACK = 6;
private:
    void* rsp_;
};

extern "C" void switchContext(void* thisRsp, void* otherRsp);
extern "C" void* getCurrentContextRbp();

#endif // EXECUTION_CONTEXT_H

