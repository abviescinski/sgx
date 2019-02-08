#include "client.hpp"

socket_cliente::socket_cliente(){
	
};

SSL_CTX* socket_cliente::InitCTX(void)
{   const SSL_METHOD *method;
    SSL_CTX *ctx;
 
    OpenSSL_add_all_algorithms(); // carregar e registrar todos os criptos, etc.
    SSL_load_error_strings();  // carregar todas as mensagens de erro
    method = TLSv1_2_server_method();// criar nova instância de método de cliente
    ctx = SSL_CTX_new(method);  // criar novo contexto a partir do método
    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    cout << "finalizou ctx\n";
    return ctx;
}
 
void socket_cliente::ShowCerts(SSL* ssl)
{   X509 *cert;
    char *line;
 
    cert = SSL_get_peer_certificate(ssl); // obter o certificado do servidor
    if ( cert != NULL )
    {
        printf("Certificados de servidor:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Sujeito: %s\n", line);
        free(line);       
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Emissora: %s\n", line);
        free(line);       
        X509_free(cert); // liberar a cópia do certificado malloc
    }
    else
        printf("Info: nenhum configurado.\n");
}


//////////////////////////////////////////////////////////////////////////////////////
int socket_cliente::inicia_conexao(int argc, char **argv){
	// Cria um soquete para o cliente
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
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
        close(sockfd);
        return EXIT_FAILURE;
    }
    else
    {
		return sockfd;
	}
}

void socket_cliente::fecha_conexao(){

	strcpy(buffer, "bye ");
    send(sockfd, buffer, 3,0);
    
	close(sockfd);

	cout <<"Conexão fechada" << endl;

}
