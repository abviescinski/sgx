#include "server.hpp"

int main ()
{
	socket_server soquete;
	MYSQL banco = soquete.conecta_bd();
	int canal, clientefd;
	int id_process; //resposta é o valor de retorno para as chamadas read () e write (); isto é, contém o número de caracteres lidos ou escritos.
	struct sockaddr_in client;// Estruturas de soquete de cliente e servidor
	socklen_t client_len = sizeof(client); //armazena o tamanho do endereço do cliente. Isso é necessário para a chamada do sistema aceito.
	
	SSL_CTX *ctx;
	SSL *ssl;
	
	SSL_library_init();// Inicialize a biblioteca SSL
	ctx = soquete.InitServerCTX(); // inicializar o SSL
	soquete.LoadCertificates(ctx, "certificate.pem", "privkey.pem"); // carrega certificado
	canal = soquete.cria_s(banco);
	
	while(true){
		clientefd = accept(canal,(struct sockaddr *)&client, &client_len);
		/*if (clientefd < 0) cerr <<"Não pode aceitar:";
		id_process = fork();
		if (id_process < 0) cerr <<"ERRO em fork()";
		if (id_process==0){*/
			//close(canal);
			ssl = SSL_new(ctx); // obter novo estado SSL com contexto
			cout <<"ssl: " << ssl;
			SSL_set_fd(ssl, clientefd); // definir o soquete de conexão para o estado SSL
			soquete.conversa_s(banco, ssl);
			exit (0);
		/*}
		else 
		{*/
			close(clientefd);
			SSL_CTX_free(ctx);
		//}
	}
	
	soquete.encerra_s();
}
