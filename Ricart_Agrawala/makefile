tcp: tcp.hpp tcp.cpp
	g++ -c  tcp.cpp -o tcp.o

timeutil: timeutil.hpp timeutil.cpp
	g++ -c timeutil.cpp -o timeutil.o

message: message.hpp message.cpp
	g++ -c message.cpp -o message.o

criticalsection: timeutil criticalsection.hpp criticalsection.cpp
	g++ -c criticalsection.cpp -o criticalsection.o


main: main.cpp tcp.o timeutil.o message.o criticalsection.o
	g++ -o main.out main.cpp tcp.o timeutil.o message.o criticalsection.o -lpthread

runservers: run_distributed_system.cpp main timeutil.o
	g++ -o runservers.out run_distributed_system.cpp timeutil.o