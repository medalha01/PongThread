#include <iostream>
#include "thread.h"
#include "traits.h"
#include "list.h"

__BEGIN_API

int Thread::id_recente = -1;
Thread * Thread::_running = nullptr;
Thread Thread::_main;
Thread Thread::_dispatcher;
Thread::Ready_Queue Thread::_ready;
CPU::Context Thread::_main_context;
Ordered_List<Thread> Thread::_suspended;

int Thread::switch_context(Thread * prev, Thread * next) {
    db<Thread>(TRC)<<"Thread::switch_context() chamado\n";
    if (prev != nullptr && next != nullptr && prev != next) {
        _running = next;
        return CPU::switch_context((prev->_context), (next->_context));
    } else {
        db<Thread>(ERR)<<"Thread::switch_context() apresentou erro\n";
        return -1;
    }
}

void Thread::thread_exit (int exit_code) {
    db<Thread>(TRC)<<"Thread::thread_exit() chamado\n";
    _state = FINISHING;
    _exit_code = exit_code;

    if (_waiting)
    {
        Thread *temp_resume = _waiting;
        _waiting = nullptr;

        temp_resume->resume();
    }
    
    yield();
}

int Thread::id() {
    return this->_id;
}

Thread::Context *Thread::context()
{
	db<Thread>(TRC) << "Thread::context()\n";
	return _context;
}

void Thread::init(void (*main)(void *)) {
    db<Thread>(TRC)<<"Thread::init() chamado\n";
    std::string nome = "main";
	new (&_main) Thread((void (*)(char *))main, (char *)nome.data());
    new (&_main_context) CPU::Context();
	new (&_dispatcher) Thread(Thread::dispatcher);

    _running = &_main;
	_main._state = RUNNING;
	_ready.remove(&_main._link);
    db<Thread>(TRC)<<"Thread::init() finalizado\n";
	CPU::switch_context(&_main_context, _main.context());
}

void Thread::dispatcher() {
    db<Thread>(TRC)<<"Thread::dispatcher() chamado\n";
    int size = _ready.size();
    while (size>0) {
        Thread *next_running = Thread::_ready.remove_head()->object();
        _dispatcher._state = READY;
        _ready.insert(&_dispatcher._link);
        _running = next_running;
        _running->_state = RUNNING;
        switch_context(&_dispatcher, _running);
        if (next_running->_state == FINISHING) {
            _ready.remove(next_running);
        }
        size = _ready.size();
    }
    db<Thread>(TRC) << "_ready.size(): " << Thread::_ready.size() << "\n";
    Thread::switch_context(&Thread::_dispatcher, &Thread::_main);
}

void Thread::yield() {
    db<Thread>(TRC)<<"Thread::yield() chamado\n";
    Thread *next = Thread::_ready.remove_head()->object();
    if (_running->_state != FINISHING && _running->_state != SUSPENDED)
    {
        int now = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        _running->_link.rank(now);
    }
    if (_running->_state == RUNNING) {
        _running->_state = READY;
    }
    if (_running->_state != FINISHING && _running->_state != SUSPENDED && _running->_state != WAITING)
    {
        _ready.insert(&(_running->_link));
    }

    Thread * temp = _running;
    _running = next;
    _running->_state = RUNNING;
    _ready.remove(_running);    // remove a próxima a executar
    db<Thread>(TRC)<<"Thread::yield() encerrado\n";
    switch_context(temp, _running);
}

Thread::~Thread() {
    db<Thread>(TRC) << "Thread::~Thread() chamado\n";
    delete this->_context;
}

/*
 * Método join()
 */
int Thread::join() {
    db<Thread>(TRC) << "Thread::join() chamado\n";
    if (this->_state != FINISHING && this != _running && _waiting == nullptr) {
        this->_waiting = _running;
        _running->suspend();
    } else {
        db<Thread>(ERR)<<"ERRO EM Thread::join()\n";
        return -1;
    }
    return _exit_code;
}

/*
 * Método suspend()
 */
void Thread::suspend() {
    db<Thread>(TRC) << "Thread::suspend() chamado\n";
    if (this != _running) {
        _suspended.insert(&_link);
        _ready.remove(this);
    }
    this->_state = SUSPENDED;
    if (_running == this)
    {
        yield();
    }
}

/*
 * Método resume()
 */
void Thread::resume() {
    db<Thread>(TRC) << "Thread::resume() chamado\n";

    if (this->_state == SUSPENDED) {
        _suspended.remove(&this->_link);
        _ready.insert(&(this->_link));
        this->_state = READY;
    }
}

void Thread::wakeup_state() {
    this->_state = READY;
    _ready.insert(&(this->_link));
}

void Thread::wakeup(Thread *to_awake)
{
	to_awake->_state = READY;
	_ready.insert(&to_awake->_link);
	yield();
}

void Thread::set_state(Thread::State state)
{
	_state = state;
}


__END_API