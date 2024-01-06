CC=g++

CFLAGS=-c -std=c++17 -Wall -Wextra

all: traceroute

traceroute: traceroute.o sender.o receiver.o
	$(CC) traceroute.o sender.o receiver.o -o traceroute

traceroute.o: traceroute.cpp
	$(CC) $(CFLAGS) traceroute.cpp 

sender.o: sender.cpp sender.hpp
	$(CC) $(CFLAGS) sender.cpp sender.hpp

receiver.o: receiver.cpp receiver.hpp
	$(CC) $(CFLAGS) receiver.cpp receiver.hpp

distclean: clean
	$(RM) traceroute 

clean:
	$(RM) *o *gch
