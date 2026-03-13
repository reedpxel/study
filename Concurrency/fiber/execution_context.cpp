#include "execution_context.hpp"

ExecutionContext::ExecutionContext(void* rsp)
        : rspValue(rsp)
{}

void* ExecutionContext::getStackTop()
{
    return rspValue;
}

void ExecutionContext::setStackTop(void* rsp)
{
    rspValue = rsp;
}

