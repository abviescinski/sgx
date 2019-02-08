all: server.o main.o 
	g++ -o main server.o main.o -lmysqlclient

server.o: server.cpp server.hpp
	g++ -c server.cpp
	
main.o: main.cpp server.hpp
	g++ -c main.cpp

clean:
	rm -rf *.o
