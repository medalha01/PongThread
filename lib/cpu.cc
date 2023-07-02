#include "cpu.h"
#include "traits.h"
#include "debug.h"
#include <iostream>

__BEGIN_API

void CPU::Context::save()
{
    //adicionar implementação
    db<CPU>(TRC)<<"CPU::save() chamado\n";
    getcontext(&(this->_context));
}

void CPU::Context::load()
{
    //adicionar implementação
    db<CPU>(TRC)<<"CPU::load() chamado\n";
    setcontext(&(this->_context));
}

CPU::Context::~Context()
{
    //adicionar implementação
    db<CPU>(TRC)<<"CPU::~Context() chamado\n";
    if (this->_stack != nullptr) {
        delete[] (this->_stack);
    } else {
        db<CPU>(WRN)<<"CPU::~Context() tentou deletar ponteiro nullptr\n";
    }
}

int CPU::switch_context(Context *from, Context *to)
{
    //implementação do método
    db<CPU>(TRC)<<"CPU::switch_context() chamado\n";
    return swapcontext(&from->_context, &to->_context);
}

int CPU::finc(volatile int & number) {
    db<Thread>(TRC)<<"CPU::finc() chamado\n";
    int sum = 1;
    __asm__ __volatile__("lock xadd %0, %2;"
                        : "=a"(sum)
                        : "a"(sum), "m"(number)
                        );
    return sum;
}

int CPU::fdec(volatile int & number) {
    db<Thread>(TRC)<<"CPU::fdec() chamado\n";
    int sum = -1;
    __asm__ __volatile__("lock xadd %0, %2;"
                        : "=a"(sum)
                        : "a"(sum), "m"(number)
                        );
    return sum;
}

__END_API
