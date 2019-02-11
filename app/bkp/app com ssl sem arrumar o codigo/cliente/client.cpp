#include "client.hpp"

socket_cliente::socket_cliente(){
	
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SSL_CTX* socket_cliente::InitCTX(void)
{   const SSL_METHOD *method;
    SSL_CTX *ctx;
cout << "Inicio cliente CTX, carregar as coisas do OpenSSL\n";
    OpenSSL_add_all_algorithms();  /* Load cryptos, et.al. */
    SSL_load_error_strings();   /* Bring in and register error messages */
    method = TLS_client_method();  /* Create new client-method instance */
    ctx = SSL_CTX_new(method);   /* Create new context */
cout << "Finalizou os carregamentos SSL, verifica se deu tudo certo.\n";
    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
cout << "Finaliza a função Init e retorna ctx.\n";
    return ctx;
}
 
void socket_cliente::ShowCerts(SSL* ssl)
{   X509 *cert;
    char *line;
cout << "Apresenta certificados \n";
    cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
cout << "Obter certificados (se disponíveis)\n";
    if ( cert != NULL )
    {
		cout << "\n -------INFORMACOES DO CERTIFICADO ------- \n";
        cout << "Server certificates:\n";
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        cout << "Subject: "<< line<<endl;
        free(line);       /* free the malloc'ed string */
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        cout << "Issuer: "<< line <<endl;
        free(line);       /* free the malloc'ed string */
        X509_free(cert);     /* free the malloc'ed certificate copy */
    }
    else
        cout << "Info Cliente: Nenhum certificado de cliente configurado.\n";
}
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

	bzero((char*)&server, sizeof(server));
	
    // Define as propriedades da conexão
    server.sin_family = PF_INET;
    server.sin_port = htons(PORT);
    bcopy((char *)endereco->h_addr, (char*)&server.sin_addr.s_addr, endereco->h_length);

    // Tenta se conectar ao servidor
    if (connect(sockfd, (struct sockaddr*)&server, sizeof(server)) == -1) {
        cerr <<"Não pode se conectar ao servidor";
        return EXIT_FAILURE;
    }
	return sockfd;

}

void socket_cliente::fecha_conexao(){

	strcpy(buffer, "bye ");
    send(sockfd, buffer, 3,0);
    
	close(sockfd);

	cout <<"Conexão fechada" << endl;

}
