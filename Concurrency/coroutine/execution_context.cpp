#include "execution_context.hpp"

ExecutionContext::ExecutionContext(void* rsp/* = nullptr */)
        : rsp_(rsp)
{}

void ExecutionContext::switchTo(ExecutionContext& other)
{
    switchContext(&this->rsp_, &other.rsp_);
}

void ExecutionContext::setRsp(void* rsp)
{
    rsp_ = rsp;
}

void* ExecutionContext::getRsp()
{
    return rsp_;
}

