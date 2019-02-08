#include "client.hpp"

socket_cliente::socket_cliente(){
	
};

int socket_cliente::inicia_conexao(int argc, char **argv){

    // Cria um soquete para o cliente
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        cerr << "Erro na criação do soquete do cliente.";
    }
        
    endereco = gethostbyname(argv[1]);
    if (endereco == NULL) {
        cerr << "ERRO, servidor nao encontrado " << stderr << endl;
        return EXIT_FAILURE;
    }

	bzero((char *) &server, sizeof(server));
	
    // Define as propriedades da conexão
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    bcopy((char *)endereco->h_addr, (char *)&server.sin_addr.s_addr, endereco->h_length);

    // Tenta se conectar ao servidor
    if (connect(sockfd, (struct sockaddr*) &server, sizeof(server)) == -1) {
        cerr <<"Não pode se conectar ao servidor";
        return EXIT_FAILURE;
    }
    else
    {
		// Recebe a mensagem de apresentação do servidor
		if ((res_ser = recv(sockfd, buffer, LEN, 0)) > 0) {
			buffer[res_ser + 1] = '\0';
			cout << "\n" << buffer << endl;
		}
		return sockfd;
	}
}

void socket_cliente::fecha_conexao(){

	strcpy(buffer, "bye ");
    send(sockfd, buffer, 3,0);
    
	close(sockfd);

	cout <<"Conexão fechada" << endl;

}
