CC=g++

CFLAGS=-c -std=c++17 

all: transport

transport: main.o Transport.o TimeoutManager.o Window.o
	$(CC) main.o Transport.o TimeoutManager.o Window.o -o transport

main.o: main.cpp 
	$(CC) $(CFLAGS) main.cpp 

Transport.o: Transport.cpp Transport.hpp
	$(CC) $(CFLAGS) Transport.cpp Transport.hpp

TimeoutManager.o: TimeoutManager.cpp TimeoutManager.hpp
	$(CC) $(CFLAGS) TimeoutManager.cpp TimeoutManager.hpp

Window.o: Window.cpp Window.hpp
	$(CC) $(CFLAGS) Window.cpp Window.hpp

distclean: clean
	$(RM) transport

clean:
	$(RM) *o *gch