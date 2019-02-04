#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

/* Define a porta do servidor */
#define PORT 4242

/* Tamanho de buffer do socket */
#define BUFFER_LENGTH 4096

enum tipo{
	corrente,
	poupanca,
	salario
};

typedef struct conta{
	int id;
	char[50] dono; 
	float saldo;
}ct1,ct2;

/* Execução principal do programa servidor */
int main(void) 
{

    /* Estruturas de soquete de cliente e servidor */
    struct sockaddr_in client, server;

    /* Descritores de arquivo de cliente e servidor */
    int serverfd, clientfd;

    char buffer[BUFFER_LENGTH];

    fprintf(stdout, "Iniciando servidor\n");

    /* Cria um soquete IPv4 */
    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    if(serverfd == -1) {
        perror("Nao pode criar o servidor socket: ");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "Servidor soquete criado com fd: %d\n", serverfd);


    /* Define as propriedades do soquete do servidor */
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    memset(server.sin_zero, 0x0, 8);


    /* Lidar com o erro da porta já em uso */
    int yes = 1;
    if(setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR,
                  &yes, sizeof(int)) == -1) {
        perror("Erro de opcoes de soquete:");
        return EXIT_FAILURE;
    }


    /* ligar o soquete a uma porta */
    if(bind(serverfd, (struct sockaddr*)&server, sizeof(server)) == -1 ) {
        perror("Erro de ligacao do soquete:");
        return EXIT_FAILURE;
    }


    /* Começa a esperar conexões de clientes */
    if(listen(serverfd, 1) == -1) {
        perror("Erro de escuta:");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "Escutando na porta %d\n", PORT);

    socklen_t client_len = sizeof(client);
    if ((clientfd=accept(serverfd,
        (struct sockaddr *) &client, &client_len )) == -1) {
        perror("Não pode aceitar:");
        return EXIT_FAILURE;
    }


    /* Cópias em buffer nossa mensagem de boas vindas */
    strcpy(buffer, "Ola cliente!\n\0");


    /* Envia a mensagem para o cliente */
    if (send(clientfd, buffer, strlen(buffer), 0)) {
        fprintf(stdout, "Cliente conectado.\n Aguardando mensagem ...\n");

		
        /* Comunica-se com o cliente até que a mensagem do bye vem */
        /*do {

            // Zerando Buffers 
            memset(buffer, 0x0, BUFFER_LENGTH);

            // Recebe mensagem do cliente 
            int message_len;
            if((message_len = recv(clientfd, buffer, BUFFER_LENGTH, 0)) > 0) {
                buffer[message_len - 1] = '\0';
                printf("Cliente disse: %s\n", buffer);
            }


            // mensagem 'bye' termina a conexão 
            if(strcmp(buffer, "bye") == 0) {
                send(clientfd, "bye", 3, 0);
            } else {
                send(clientfd, "ok\n", 4, 0);
            }

        } while(strcmp(buffer, "bye"));*/
        
     }

    /* Conexão do cliente Fechar */
    close(clientfd);

    /* Feche o soquete local */
    close(serverfd);

    printf("Conexao encerrada\n\n");

    return EXIT_SUCCESS;
}
