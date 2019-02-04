#include "client.hpp"

socket_cliente::socket_cliente(){
	
};

int socket_cliente::inicia_conexao(){
    
    // Cria um soquete para o cliente
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Erro na criação do soquete do cliente.");
    }
    fprintf(stdout, "Soquete do cliente criado com fd: %d\n", sockfd);

    // Define as propriedades da conexão
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    memset(server.sin_zero, 0x0, 8);

    // Tenta se conectar ao servidor
    if (connect(sockfd, (struct sockaddr*) &server, len) == -1) {
        perror("Não pode se conectar ao servidor");
    }
    
    // Recebe a mensagem de apresentação do servidor
    if ((slen = recv(sockfd, buffer, LEN, 0)) > 0) {
        buffer[slen + 1] = '\0';
        fprintf(stdout, "\n%s\n", buffer);
    }
    
    return sockfd;
}

void socket_cliente::fecha_conexao(){

	strcpy(buffer, "bye ");
    send(sockfd, buffer, 3,0);
    
	close(sockfd);

	cout <<"Conexão fechada" << endl;

}
