#ifndef traits_h
#define traits_h

//Não alterar as 3 declarações abaixo

#define __BEGIN_API    \
    namespace SOLUTION \
    {
#define __END_API }
#define __USING_API using namespace SOLUTION;

__BEGIN_API

class CPU; //declaração das classes criadas nos trabalhos devem ser colocadas aqui
class Debug;
class Thread;
class System;
class Lists;
class Semaphore;

//declaração da classe Traits
template<typename T>
struct Traits {
    static const bool debugged = false;
};

template<> struct Traits<Debug>: public Traits<void>
{
    static const bool error = false;
    static const bool warning = false;
    static const bool info = false;
    static const bool trace = false;
};

template<> struct Traits<CPU>
{
    //Adicionar aqui a declaração do tamanho da Pilha
    static const int STACK_SIZE = 96000;
    static const bool debugged = true;
};

template<> struct Traits<System>: public Traits<void>
{
    static const bool debugged = true;
};

template<> struct Traits<Thread>: public Traits<void>
{
    static const bool debugged = true;
};

template <> struct Traits<Lists> : public Traits<void>
{
	static const bool debugged = false;
};

template <> struct Traits<Semaphore> : public Traits<void>
{
    static const bool debugged = true;
};

__END_API

#endif
