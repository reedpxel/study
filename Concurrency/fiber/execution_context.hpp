#ifndef EXECUTION_CONTEXT_H
#define EXECUTION_CONTEXT_H
#pragma once

#include <cstddef>

class ExecutionContext
{
public:
    ExecutionContext() = default;
    ExecutionContext(void* rsp);
    void* getStackTop();
    void setStackTop(void* rsp);

    const static size_t CALLEE_SAVED_REGISTERS = 7;
    // the amount of callee-saved registers that are stored on fiber's stack 
    // when fiber is not being run
    const static size_t CALLEE_SAVED_REGISTERS_STACK = 6; 

private:
   void* rspValue = nullptr;
};

// saves callee-saved registers' values on current stack, switches to next 
// stack and writes values on top of the next stack in registers
extern "C" void switchContextAsm(void* currentRsp, void* nextRsp);
// switches stacks, does not change callee-saved registers
extern "C" void switchStackAsm(void* currentRsp, void* nextRsp);

#endif

