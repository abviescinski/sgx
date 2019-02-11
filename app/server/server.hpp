#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <errno.h>

#include <arpa/inet.h>
#include <resolv.h>
#include "openssl/ssl.h"
#include "openssl/err.h"

#include <mysql/mysql.h>

#define PORT 4242
#define BUFFER_LENGTH 128

#define HOST "localhost"
#define USER "amanda"
//#define PASS "0909"
#define PASS "Abv0909*"
#define DB "banco"

#define Saldo 10
#define IDcliente 20
#define NovoCliente 30
#define NovaConta 40
#define IDconta 50
#define Nome 60
#define Login 70
#define Soma 80
#define Subtrai 90

using namespace std;

class socket_server{
	
	public:
		socket_server();
		
		SSL_CTX* InitServerCTX(void);
		void LoadCertificates(SSL_CTX* ctx, const char* CertFile, const char* KeyFile);
		void ShowCerts(SSL* ssl);
		
		int cria_s(MYSQL banco);
		void conversa_s(MYSQL banco, SSL* ssl);
		void encerra_s();
	
	struct sockaddr_in server;// Estruturas de soquete de cliente e servidor
	int serverfd; // Descritores de arquivo de cliente e servidor
    char buffer_server[BUFFER_LENGTH];// Descritores de arquivo de cliente e servidor
	
};

class str_banco{
	
	public:
	str_banco();
	MYSQL conecta_bd();
	
	bool insert_bd(MYSQL banco, int opc, char info[5][BUFFER_LENGTH]);
	MYSQL_ROW select_bd(MYSQL banco, int opc, char info[BUFFER_LENGTH]);
	MYSQL_ROW innerjoin_bd(MYSQL banco, int opc, char info[2][BUFFER_LENGTH]);
	bool update_bd(MYSQL banco, int opc, char info[2][BUFFER_LENGTH]);
	bool delet_bd(MYSQL banco, int opc);
	
	int res_banco;
	char solicita[BUFFER_LENGTH];
	MYSQL_ROW dados_banco;
	MYSQL_RES *res_consulta;
	
};

class menu_um{
	
	public:
		menu_um();
		void criar_conta(SSL* ssl, MYSQL banco);
		void remover(SSL* ssl, MYSQL banco);
		void acessar(SSL* ssl, MYSQL banco);
		void depositar(SSL* ssl, MYSQL banco);
	
	str_banco str_bd;
	char buffer_respostas[BUFFER_LENGTH];
	int  contador, res_banco;
	MYSQL_ROW dados_banco;
	MYSQL_RES *res_consulta;
};

class menu_dois{
	
	public:
		menu_dois();
		void obtem_saldo(SSL* ssl, MYSQL banco, char dados_acesso[BUFFER_LENGTH]);
		void sacar(SSL* ssl, MYSQL banco, char dados_acesso[3][BUFFER_LENGTH]);
		void transferir(SSL* ssl, MYSQL banco, char dados_acesso[3][BUFFER_LENGTH]);
		void extrato(SSL* ssl, MYSQL banco, char dados_acesso[3][BUFFER_LENGTH]);
	
	str_banco str_bd;
	int  contador, res_banco;
	char buffer_respostas[BUFFER_LENGTH];
	MYSQL_ROW dados_banco;
	MYSQL_RES *res_consulta;
};
