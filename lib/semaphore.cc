#include "semaphore.h"

__BEGIN_API

Semaphore::Semaphore(int v) {
    db<Thread>(TRC)<<"Semaphore::Semaphore() construtor chamado\n";
    this->_value = v;
    this->_waiting_queue = new Waiting_Queue();
}

Semaphore::~Semaphore() {
    db<Thread>(TRC)<<"Semaphore::~Semaphore() destrutor chamado\n";
    wakeup_all();
    delete _waiting_queue;
}

/*
 * Método Sleep atômico utilizando fdec
 */
void Semaphore::p() {
    db<Thread>(TRC)<<"Semaphore::p() chamado\n";
    if (fdec(_value) < 1) {
        sleep();
        //p();
    }
    // else {
    //     fdec(this->_value);
    // }
}

/*
 * Método Wakeup atômico utilizando finc
 */
void Semaphore::v() {
    db<Thread>(TRC)<<"Semaphore::v() chamado\n";
    if (finc(this->_value) < 0) {
        //finc(_value);
        wakeup();
    }
    // else {
    //     finc(this->_value);
    // }
}

void Semaphore::wakeup() {
    db<Thread>(TRC)<<"Semaphore::wakeup() chamado\n";
    if (_waiting_queue->size() > 0) {
        //Thread *next_running = Thread::_ready.remove_head()->object();
        SOLUTION::List_Elements::Doubly_Linked_Ordered<SOLUTION::Thread, SOLUTION::List_Element_Rank> * temp_head = _waiting_queue->remove_head();
        if (temp_head) {
            Thread * temp_wakeup = temp_head->object();
            temp_wakeup->wakeup_state();
        }
        //temp_wakeup->yield();
    }
}

void Semaphore::wakeup_all() {
    db<Thread>(TRC)<<"Semaphore::wakeup_all() chamado\n";
    while (!_waiting_queue->empty()) {
        wakeup();
    }
}

void Semaphore::sleep()
{
    db<Thread>(TRC) << "Semaphore::sleep()\n";
    Thread::running()->set_state(Thread::State::WAITING);

    Waiting_Queue::Element link = Waiting_Queue::Element(Thread::running(), (std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()));

    _waiting_queue->insert(&link);
    Thread::yield();
}

int Semaphore::finc(volatile int & number) {
    db<Thread>(TRC)<<"Semaphore::finc() chamado\n";
    return CPU::finc(number);
}

int Semaphore::fdec(volatile int & number) {
    db<Thread>(TRC)<<"Semaphore::fdec() chamado\n";
    return CPU::fdec(number);
}

__END_API