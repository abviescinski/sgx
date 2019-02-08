#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#include <mysql/mysql.h>

#include "openssl/ssl.h"
#include "openssl/err.h"
#include <resolv.h>

#define HOST "localhost"
#define USER "amanda"
#define PASS "Abv0909*"
#define DB "banco"

/* Define a porta do servidor */
#define PORT 4242

/* Tamanho de buffer do socket */
#define BUFFER_LENGTH 128

using namespace std;

class socket_server{
	
	public:
		socket_server();
		int cria_s(MYSQL banco);
		void conversa_s(MYSQL banco, SSL *ssl);
		void encerra_s();
		
		MYSQL conecta_bd();
		
		//Funções ssl
		SSL_CTX* InitServerCTX(void);
		void LoadCertificates(SSL_CTX* ctx, const char* CertFile, const char* KeyFile);
		void ShowCerts(SSL* ssl);
	
	
	int serverfd; // Descritores de arquivo de cliente e servidor (serverfd usada em cria_s)
	struct sockaddr_in server; // Estruturas de soquete de cliente e servidor (server usada em cria_s)
    
    char buffer_serv[BUFFER_LENGTH];// Descritores de arquivo de cliente e servidor
	
};

class menu_um{
	
	public:
		menu_um();
		void criar_conta(int client_fd, MYSQL banco);
		void remover(int client_fd, MYSQL banco);
		void acessar(int client_fd, MYSQL banco);
		void depositar(int client_fd, MYSQL banco);
		
	char buffer_respostas[BUFFER_LENGTH];
	char dados_cliente[5][BUFFER_LENGTH];
	int  contador, res_banco;
	char str_banco[BUFFER_LENGTH];
	MYSQL_ROW dados_banco;
	MYSQL_RES *res_consulta;
};

class menu_dois{
	
	public:
		menu_dois();
		void opcoes_dois(int client_fd, MYSQL banco, char dados_acesso[3][BUFFER_LENGTH]);
		void obtem_saldo(int client_fd, MYSQL banco, char dados_acesso[3][BUFFER_LENGTH]);
		void sacar(int client_fd, MYSQL banco, char dados_acesso[3][BUFFER_LENGTH]);
		void transferir(int client_fd, MYSQL banco, char dados_acesso[3][BUFFER_LENGTH]);
		void extrato(int client_fd, MYSQL banco, char dados_acesso[3][BUFFER_LENGTH]);
	
	int  contador, res_banco;
	char dados_cliente[5][BUFFER_LENGTH], buffer_respostas[BUFFER_LENGTH];
	char str_banco[BUFFER_LENGTH];
	MYSQL_ROW dados_banco;
	MYSQL_RES *res_consulta;
};
/*
class banco_str{
	
	MYSQL conecta_bd();
	
	
	
	
	MYSQL_ROW dados_banco;
	MYSQL_RES *res_consulta;
	
	
}*/
