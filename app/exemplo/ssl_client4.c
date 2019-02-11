#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <malloc.h>
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
printf("18: iniciou criação do socket\n");
    if ( (host = gethostbyname(hostname)) == NULL )
    {
        perror(hostname);
        abort();
    }
    sd = socket(PF_INET, SOCK_STREAM, 0);
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = *(long*)(host->h_addr);
printf("29: criou cabeçalhos socket \n");
    if ( connect(sd, (struct sockaddr*)&addr, sizeof(addr)) != 0 )
    {
        close(sd);
        perror(hostname);
        abort();
    }
printf("36: finalizou criação do socket, conexao com servidor tudo ok \n");
    return sd;
}
 
SSL_CTX* InitCTX(void)
{   const SSL_METHOD *method;
    SSL_CTX *ctx;
printf("43: Inicio cliente CTX, carregar as coisas do OpenSSL\n");
    OpenSSL_add_all_algorithms();  /* Load cryptos, et.al. */
    SSL_load_error_strings();   /* Bring in and register error messages */
    method = TLS_client_method();  /* Create new client-method instance */
    ctx = SSL_CTX_new(method);   /* Create new context */
printf("48: Finalizou os carregamentos SSL, verifica se deu tudo certo.\n");
    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
printf("54: Finaliza a função Init e retorna ctx.\n");
    return ctx;
}
 
void ShowCerts(SSL* ssl)
{   X509 *cert;
    char *line;
 printf("61: Apresenta certificados \n");
    cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
printf("63: Obter certificados (se disponíveis)\n");
    if ( cert != NULL )
    {
		printf("\n -------INFORMACOES DO CERTIFICADO ------- \n");
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line);       /* free the malloc'ed string */
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);       /* free the malloc'ed string */
        X509_free(cert);     /* free the malloc'ed certificate copy */
    }
    else
        printf("77: Info Cliente: Nenhum certificado de cliente configurado.\n");
}
 
int main(int count, char *strings[])
{   SSL_CTX *ctx;
    int server;
    SSL *ssl;
    char buf[1024];
	char acClientRequest[1024] ={0};
    int bytes;
    char *hostname, *portnum;
printf("88: INICIO \n");
    if ( count != 3 )
    {
        printf("usage: %s <hostname> <portnum>\n", strings[0]);
        exit(0);
    }
    SSL_library_init();
printf("95: Iniciou a biblioteca SSL\n");
    hostname=strings[1];
    portnum=strings[2];
 
    ctx = InitCTX();
    server = OpenConnection(hostname, atoi(portnum));
    ssl = SSL_new(ctx);      /* create new SSL connection state */
    SSL_set_fd(ssl, server);    /* attach the socket descriptor */
printf("210: criou cliente socket, cliente ssl.\n");
    if ( SSL_connect(ssl) == FAIL )   /* perform the connection */
        ERR_print_errors_fp(stderr);
    else
    {  
 printf("108: solicitacao de conexao com servidor aceita\n");
        char acUsername[16] ={0};
		char acPassword[16] ={0};
		const char *cpRequestMessage = "UserName: %s \nPassword: %s"; 
 
        printf("Enter the User Name : ");
		scanf("%s",acUsername);
 
		printf("\n\nEnter the Password : ");
		scanf("%s",acPassword);
 
        sprintf(acClientRequest, cpRequestMessage, acUsername,acPassword);   /* construct reply */
printf("120: Criou string para encaminhar ao servidor.\n");
printf("String: %s.\n",acClientRequest);
        printf("\nConnected with %s encryption\n", SSL_get_cipher(ssl));
        ShowCerts(ssl);        /* get any certs */
printf("123: apresentou certificados e a conexao encriptada.\n");
        SSL_write(ssl,acClientRequest, strlen(acClientRequest));   /* encrypt & send message */
printf("125: enviou mensagem ao servidor .\n");
        bytes = SSL_read(ssl, buf, sizeof(buf)); /* get reply & decrypt */
        buf[bytes] = 0;
        printf("Servidor disse: \"%s\"\n", buf);
        SSL_free(ssl);        /* release connection state */
    }
printf("131: finaliza conexoes .\n");
    close(server);         /* close socket */
    SSL_CTX_free(ctx);        /* release context */
    return 0;
}
