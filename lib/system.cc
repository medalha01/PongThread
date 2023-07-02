#include "system.h"
#include "traits.h"
#include "debug.h"
#include "thread.h"

__BEGIN_API

void System::init(void (*main)(void *)) {
    db<System>(TRC)<<"System::init() chamado\n";
    setvbuf(stdout, 0, _IONBF, 0);
    Thread::init(main);
}

__END_API
