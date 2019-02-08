#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <resolv.h>
#include "openssl/ssl.h"
#include "openssl/err.h"
 
#define FAIL    -1
 
// Crie o soquete SSL e inicialize a estrutura de endereço do soquete
int OpenListener(int port)
{   int sd;
    struct sockaddr_in addr;
 
    sd = socket(PF_INET, SOCK_STREAM, 0);
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0 )
    {
        perror("não pode ligar porta");
        abort();
    }
    if ( listen(sd, 10) != 0 )
    {
        perror("Não é possível configurar a porta de escuta");
        abort();
    }
    return sd;
}

SSL_CTX* InitServerCTX(void)
{   const SSL_METHOD *method;
    SSL_CTX *ctx;
 
    OpenSSL_add_all_algorithms();  // carregar e registrar todos os criptos, etc.
    SSL_load_error_strings(); // carregar todas as mensagens de erro
    method = TLS_server_method();  // criar nova instância de método de servidor
    ctx = SSL_CTX_new(method);   // criar novo contexto a partir do método
    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    return ctx;
}
 
void LoadCertificates(SSL_CTX* ctx, char* CertFile, char* KeyFile)
{
    // definir o certificado local do CertFile 
    if (SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM)<=0)
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    // definir a chave privada do KeyFile (pode ser o mesmo que CertFile) 
    if (SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM)<=0)
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    // verifica chave privada 
    if (!SSL_CTX_check_private_key(ctx))
    {
        fprintf(stderr, "Chave privada não corresponde ao certificado público\n");
        abort();
    }
}
 
void ShowCerts(SSL* ssl)
{   X509 *cert;
    char *line;
 
    cert = SSL_get_peer_certificate(ssl); // Obter certificados (se disponíveis)
    if ( cert != NULL )
    {
        printf("Certificados de servidor:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Sujeito: %s\n", line);
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Emissora: %s\n", line);
        free(line);
        X509_free(cert);
    }
    else
        printf("Sem certificados.\n");
}
 
void Servlet(SSL* ssl) // Servir a conexão - segmentável
{   char buf[1024] = {0};
   
    int sd, bytes;
    const char *ServerResponse=" aticleworld.com \n1.5 \nEmbedede and c c++ \namlendra\n";
	const char *cpValidMessage = "UserName: aticle \nPassword: 123 \n"; 
 
    if ( SSL_accept(ssl) == FAIL ) // o protocolo SSL aceita
        ERR_print_errors_fp(stderr);
    else
    {
        ShowCerts(ssl); // obter quaisquer certificados
        bytes = SSL_read(ssl, buf, sizeof(buf)); // obter pedido
		buf[bytes] = '\0';
 
		printf("Cliente: %s \n", buf);
 
        if ( bytes > 0 )
        {
			if(strcmp(cpValidMessage,buf) == 0)
			{
				SSL_write(ssl, ServerResponse, strlen(ServerResponse)); // envia resposta
			}
			else
				{
				SSL_write(ssl, "Mensagem Inválida", strlen("Mensagem Inválida"));
				}
		}
        else
			{
			ERR_print_errors_fp(stderr);
			}
            
    }
    sd = SSL_get_fd(ssl); // obter conexão de soquete
    SSL_free(ssl); // liberar estado SSL
    close(sd); //Encerra conexao
}
 
int main(int count, char *Argc[])
{   SSL_CTX *ctx;
 
    SSL_library_init();// Inicialize a biblioteca SSL
 
    ctx = InitServerCTX(); // inicializar o SSL
    LoadCertificates(ctx, "certificate.pem", "privkey.pem"); // carrega certificado
    server = OpenListener(atoi(portnum)); // cria servidor socket
    while (1)
    {   struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        SSL *ssl;
 
        int client = accept(server, (struct sockaddr*)&addr, &len);  // aceitar conexão como de costume
        printf("Connection: %s:%d\n",inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
        ssl = SSL_new(ctx); // obter novo estado SSL com contexto
        SSL_set_fd(ssl, client); // definir o soquete de conexão para o estado SSL
        Servlet(ssl); // conexão de serviço
    }
    close(server); // fechar soquete do servidor 
    SSL_CTX_free(ctx); // contexto de lançamento
}
