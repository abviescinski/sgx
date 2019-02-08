#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <errno.h>
#include <stdbool.h>
#include <unistd.h>

#include <netdb.h> //presente no cliente*
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <resolv.h>

#define PORT 4242 //porta do servidor
#define LEN 128 //Tamanho de buffer do socket
#define SERVER_ADDR "127.0.0.1" //Endereço IP do servidor

using namespace std;

class socket_cliente{

	public:
		socket_cliente();
		int inicia_conexao(int argc, char **argv);
		void fecha_conexao();
		
		void ShowCerts(SSL* ssl);
		SSL_CTX* InitCTX(void);
    
    struct hostent *endereco;//define o endereço ip do servidor
    struct sockaddr_in server;//Server socket
    int sockfd, res_ser; //Descritor de arquivo do cliente para o soquete local 
    char buffer[LEN];//Buffer
};
