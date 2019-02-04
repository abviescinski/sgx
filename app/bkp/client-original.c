#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>



/* Define a porta do servidor */
#define PORT 4242


/* Tamanho de buffer do socket */
#define LEN 4096


/* Endereço IP do servidor */
#define SERVER_ADDR "127.0.0.1"



/* Execução principal do programa cliente */
int main(int argc, char *argv[]) 
{

    /* Server socket */
    struct sockaddr_in server;
    /* Descritor de arquivo do cliente para o soquete local */
    int sockfd;

    int len = sizeof(server);
    int slen;

    /* Buffer de recebimento */
    char buffer_in[LEN];
    /* Enviar buffer */
    char buffer_out[LEN];


    fprintf(stdout, "Starting Client ...\n");


    /* Cria um soquete para o cliente */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error on client socket creation:");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "Client socket created with fd: %d\n", sockfd);


    /* Define as propriedades da conexão */
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    memset(server.sin_zero, 0x0, 8);


    /* Tenta se conectar ao servidor */
    if (connect(sockfd, (struct sockaddr*) &server, len) == -1) {
        perror("Can't connect to server");
        return EXIT_FAILURE;
    }


    /* Recebe a mensagem de apresentação do servidor */
    if ((slen = recv(sockfd, buffer_in, LEN, 0)) > 0) {
        buffer_in[slen + 1] = '\0';
        fprintf(stdout, "Server says: %s\n", buffer_in);
    }


    /* Comunique-se com o servidor até a mensagem de saída chegar  */
    while (true) {

        /* Zerando Buffers */
        memset(buffer_in, 0x0, LEN);
        memset(buffer_out, 0x0, LEN);

        fprintf(stdout, "Say something to the server: ");
        fgets(buffer_out, LEN, stdin);

        /* Envia a mensagem lida para o servidor através do soquete */
        send(sockfd, buffer_out, strlen(buffer_out), 0);

        /* Recebe uma resposta do servidor */
        slen = recv(sockfd, buffer_in, LEN, 0);
        printf("Server answer: %s\n", buffer_in);

        /* mensagem 'bye' termina a conexão */
        if(strcmp(buffer_in, "bye") == 0)
            break;
    }


    /* Feche a conexão com o servidor */
    close(sockfd);

    fprintf(stdout, "\nConnection closed\n\n");

    return EXIT_SUCCESS;
}
