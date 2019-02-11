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
 
#define PORT 5555
 
// Create the SSL socket and intialize the socket address structure
int OpenListener(int port)
{   int sd;
    struct sockaddr_in addr;
    
	printf("22: iniciou criação do socket\n");
    sd = socket(PF_INET, SOCK_STREAM, 0);
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    
    printf("29: criou cabeçalhos socket \n");
    
    if (bind(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0 )
    {
        perror("can't bind port");
        abort();
    }
    if ( listen(sd, 10) != 0 )
    {
        perror("Can't configure listening port");
        abort();
    }
    
    printf("42: finalizou criação do socket, testou bind e listen \n");
    
    return sd;
}
 
int isRoot()
{
    if (getuid() != 0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
 
}
SSL_CTX* InitServerCTX(void)
{   const SSL_METHOD *method;
    SSL_CTX *ctx;
    
	printf("63: Inicio server CTX, carregar as coisas do OpenSSL\n");
	
    OpenSSL_add_all_algorithms();  /* load & register all cryptos, etc. */
    SSL_load_error_strings();   /* load all error messages */
    method = TLS_server_method();  /* create new server-method instance */
    ctx = SSL_CTX_new(method);   /* create new context from method */
    
    printf("70: Finalizou os carregamentos SSL, verifica se deu tudo certo.\n");
    
    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    
    printf("78: Finaliza a função Init server e retorna ctx.\n");
    return ctx;
}
 
void LoadCertificates(SSL_CTX* ctx, char* CertFile, char* KeyFile)
{
    /* set the local certificate from CertFile */
    if ( SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    
    printf("91: Load Certificates, verifica o pem do certificado se esta correto\n");
    
    /* set the private key from KeyFile (may be the same as CertFile) */
    if ( SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    
    printf("100: Load Certificates, verifica o se pem do chave privada esta correto\n");
    
    /* verify private key */
    if ( !SSL_CTX_check_private_key(ctx) )
    {
        fprintf(stderr, "Private key does not match the public certificate\n");
        abort();
    }
    
    printf("109: Finaliza a função LoadCertificates, tudo certo ate aqui\n");
}
 
void ShowCerts(SSL* ssl)
{   X509 *cert;
    char *line;
 
 printf("116: Apresenta certificados \n");
 
    cert = SSL_get_peer_certificate(ssl); /* Get certificates (if available) */
    
    printf("120: Obter certificados (se disponíveis)\n");
    
    if ( cert != NULL )
    {
        printf("\n -------INFORMACOES DO CERTIFICADO ------- \n");
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);
        X509_free(cert);
    }
    else
        printf("134: LADO SERVIDOR, sem certificados!\n");
}
 
void Servlet(SSL* ssl) /* Serve the connection -- threadable */
{   char buf[1024] = {0};
   
    int sd, bytes;
    const char* ServerResponse = "Name: aticleworld.com \nyear: 1.5 \nBlogType: Embedede and c c++ \nAuthor: amlendra";
 
	const char *cpValidMessage = "UserName: amanda \nPassword: 123"; 
	
	printf("146: Iniciou conversa com cliente, carregou as variaveis necessarias.\n");
 
    if ( SSL_accept(ssl) == FAIL )     /* do SSL-protocol accept */
        ERR_print_errors_fp(stderr);
    else
    {
        ShowCerts(ssl);        /* get any certificates */
 printf("153: Apresentou certificado se tiver e vai receber mensagem ao cliente.\n");
        bytes = SSL_read(ssl, buf, sizeof(buf)); /* get request */
		buf[bytes] = '\0';
 
		printf("Cliente diz: \"%s\"\n", buf);
 
        if ( bytes > 0 )
        {
			 printf("161: Recebeu mensagem do cliente e vai comparar string \n");
			if(strcmp(cpValidMessage,buf) == 0)
			{
				SSL_write(ssl, ServerResponse, strlen(ServerResponse)); /* send reply */
				printf("165: enviou resposta correta ao cliente.\n");
			}
			else
				{
					SSL_write(ssl, "Invalid Message", strlen("Invalid Message")); /* send reply */
					printf("170: enviou resposta ao cliente como mensagem invalida.\n");
				}
			}
        else
			{
				ERR_print_errors_fp(stderr);
				printf("176: Obteve erros\n");
			}
            
    }
    printf("180: Finalizou conversa com cliente e encerra as conexoes");
    sd = SSL_get_fd(ssl);       /* get socket connection */
    SSL_free(ssl);         /* release SSL state */
    close(sd);          /* close connection */
}
 
int main(int count, char *Argc[])
{   SSL_CTX *ctx;
    int server;
  
 //Only root user have the permsion to run the server
    /*if(!isRoot())
    {
        printf("Precisar ser root!!");
        exit(0);
    }*/
    
    printf(" 200: Verificou se é root \n");
 
 // Iniciou a biblioteca SSL
    SSL_library_init();
	printf("204: Iniciou a biblioteca SSL\n");

    ctx = InitServerCTX();        /* initialize SSL */
    LoadCertificates(ctx, "certificate.pem", "privkey.pem"); /* load certs */
    printf("208: carregou a certificacao SSL e os certificados\n");
    server = OpenListener(PORT);    /* create server socket */
    printf("210: criou servidor socket\n");
    while (1)
    {   struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        SSL *ssl;
 
 printf("216: Entrou no while, criou as estruturas\n");
 
        int client = accept(server, (struct sockaddr*)&addr, &len);  /* accept connection as usual */
        
printf("220: Aceitou conexao com cliente\n");
        
        printf("Connection: %s:%d\n",inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
        ssl = SSL_new(ctx);              /* get new SSL state with context */
        
printf("225: obteve novo estado SSL com contexto\n");
        
        SSL_set_fd(ssl, client);      /* set connection socket to SSL state */
        Servlet(ssl);         /* service connection */
    }
printf("230: saiu do while e fechou as conexoes\n");
    close(server);          /* close server socket */
    SSL_CTX_free(ctx);         /* release context */
}
