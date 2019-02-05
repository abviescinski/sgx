#include <stdio.h> //ok
#include <string.h> //ok
#include <stdlib.h> //ok
#include <unistd.h> //ok
#include <sys/types.h> //ok
#include <sys/socket.h> //ok
#include <netinet/in.h> //ok

#include <netdb.h> 
#include <iostream> //ok
#include <errno.h> //ok

#include <mysql/mysql.h> //ok

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
		void conversa_s(MYSQL banco, int clientefd);
		void encerra_s();
		
		MYSQL conecta_bd();
	
	struct sockaddr_in client, server;// Estruturas de soquete de cliente e servidor
	socklen_t client_len = sizeof(client); //armazena o tamanho do endereço do cliente. Isso é necessário para a chamada do sistema aceito.
	int serverfd, clientfd;// Descritores de arquivo de cliente e servidor
    int id_process; //resposta é o valor de retorno para as chamadas read () e write (); isto é, contém o número de caracteres lidos ou escritos.
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
