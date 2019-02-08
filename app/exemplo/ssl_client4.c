#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
 
#define FAIL    -1
 
int OpenConnection(const char *hostname, int port)
{   int sd;
    struct hostent *host;
    struct sockaddr_in addr;
 
    if ((host = gethostbyname(hostname)) == NULL )
    {
        perror(hostname);
        abort();
    }
    sd = socket(PF_INET, SOCK_STREAM, 0);
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = *(long*)(host->h_addr);
    if (connect(sd,(struct sockaddr*)&addr, sizeof(addr)) != 0 )
    {
        close(sd);
        perror(hostname);
        abort();
    }
    return sd;
}
 
SSL_CTX* InitCTX(void)
{   const SSL_METHOD *method;
    SSL_CTX *ctx;
 
    OpenSSL_add_all_algorithms(); // carregar e registrar todos os criptos, etc.
    SSL_load_error_strings();  // carregar todas as mensagens de erro
    method = TLS_client_method();// criar nova instância de método de cliente
    ctx = SSL_CTX_new(method);  // criar novo contexto a partir do método
    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}
 
void ShowCerts(SSL* ssl)
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
        printf("Info: nenhum certificado de cliente configurado.\n");
}
 
int main(int count, char *strings[])
{   SSL_CTX *ctx;
    int server;
    SSL *ssl;
    char buf[1024];
	char acClientRequest[1024] ={0};
    int bytes;
    char *hostname, *portnum;
	//verificar argumentos
    if (count!=3)
    {
        printf("Use: %s <hostname> <portnum>\n", strings[0]);
        exit(0);
    }
    
    SSL_library_init();
    hostname=strings[1];
    portnum=strings[2];
 
    ctx = InitCTX();
    server = OpenConnection(hostname, atoi(portnum));
    ssl = SSL_new(ctx); // criar novo estado de conexão SSL
    SSL_set_fd(ssl, server); // anexar o descritor de soquete
    if ( SSL_connect(ssl) == FAIL ) // realizar a conexão
        ERR_print_errors_fp(stderr);
    else
    {  
		char acUsername[16] ={0};
		char acPassword[16] ={0};
		const char *cpRequestMessage = "UserName: %s \nPassword: %s \n"; 
 
        printf("Digite o seu nome: ");
		scanf("%s",acUsername);
 
		printf("Digite a senha: ");
		scanf("%s",acPassword);

        sprintf(acClientRequest, cpRequestMessage, acUsername,acPassword); // construir resposta
 
        printf("\nConnected with %s encryption\n", SSL_get_cipher(ssl));
        ShowCerts(ssl); // pegue qualquer certificado
        SSL_write(ssl,acClientRequest, strlen(acClientRequest));   // criptografar e enviar mensagem
        bytes = SSL_read(ssl, buf, sizeof(buf)); // obter resposta e descriptografar
        buf[bytes] = 0;
        printf("Recebido: %s \n", buf);
        SSL_free(ssl); // liberar estado de conexão
    }
    close(server);
    SSL_CTX_free(ctx);// contexto de lançamento
    return 0;
}
