CC = g++
SRCS=main.cc thread.cc debug.cc cpu.cc main_class.cc system.cc semaphore.cc
OBJS=$(subst .cc,.o,$(SRCS))

make: main

main: $(OBJS)
	$(CC) -g -o main $(OBJS)

thread.o: thread.cc thread.h cpu.h traits.h

semaphore.o: semaphore.cc semaphore.h thread.h traits.h

debug.o: debug.cc debug.h traits.h

cpu.o: cpu.cc cpu.h traits.h debug.h

main_class.o: main_class.cc main_class.h cpu.h traits.h thread.h semaphore.h

system.o: system.cc system.h traits.h debug.h

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) main
