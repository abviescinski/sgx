all: str_bd.o server.o main.o 
	g++ -o main str_bd.o server.o main.o -lmysqlclient -L/usr/lib -lssl -lcrypto

server.o: server.cpp server.hpp
	g++ -c server.cpp

str_bd.o: str_bd.cpp server.hpp
	g++ -c str_bd.cpp
	
main.o: main.cpp server.hpp
	g++ -c main.cpp

clean:
	rm -rf *.o
