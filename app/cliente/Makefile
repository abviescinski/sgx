all: menu.o client.o main.o
	g++ -o main menu.o client.o main.o -L/usr/lib -lssl -lcrypto

client.o: client.cpp client.hpp
	g++ -c client.cpp

main.o: main.cpp client.hpp
	g++ -c main.cpp

menu.o: menu.cpp menu.hpp
	g++ -c menu.cpp

clean:
	rm -rf *.o
