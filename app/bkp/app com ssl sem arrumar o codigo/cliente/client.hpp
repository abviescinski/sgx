#include <stdio.h> //ok
#include <stdlib.h> //ok
#include <string.h> //ok
#include <iostream> //ok
#include <errno.h> //ok
#include <stdbool.h>
#include <unistd.h> //ok

#include <netdb.h> //presente no cliente*
#include <sys/types.h> //ok
#include <sys/socket.h> //ok
#include <netinet/in.h> //ok
#include <arpa/inet.h>

#include <resolv.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define PORT 4242 //porta do servidor
#define LEN 128 //Tamanho de buffer do socket
#define SERVER_ADDR "127.0.0.1" //Endereço IP do servidor

using namespace std;

class socket_cliente{

	public:
		socket_cliente();
		
		SSL_CTX* InitCTX(void);
		void ShowCerts(SSL* ssl);
		
		int inicia_conexao(int argc, char **argv);
		void fecha_conexao();
    
    struct hostent *endereco;//define o endereço ip do servidor
    struct sockaddr_in server;//Server socket
    int sockfd, res_ser; //Descritor de arquivo do cliente para o soquete local 
    char buffer[LEN];//Buffer
};
