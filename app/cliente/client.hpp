#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 4242 //porta do servidor
#define LEN 128 //Tamanho de buffer do socket
#define SERVER_ADDR "127.0.0.1" //Endereço IP do servidor

using namespace std;

class socket_cliente{

	public:
		socket_cliente();
		int inicia_conexao();
		void fecha_conexao();
     
    struct sockaddr_in server;//Server socket
    int sockfd; //Descritor de arquivo do cliente para o soquete local 

    int len = sizeof(server);
    int slen;

    char buffer[LEN];//Buffer
};
