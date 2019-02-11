#include "server.hpp"

int main ()
{
   	socket_server soquete;
	MYSQL banco = soquete.conecta_bd();
	int server_fd, cliente_fd, id_process;
	struct sockaddr_in cliente;
	socklen_t clientelen = sizeof(cliente); //armazena o tamanho do endereço do cliente. Isso é necessário para a chamada do sistema aceito.
	SSL_CTX *ctx;
	SSL *ssl;
	
	SSL_library_init();
	cout << "Iniciou a biblioteca SSL\n";
	
	ctx = soquete.InitServerCTX(); // initialize SSL 
    soquete.LoadCertificates(ctx, "certificate.pem", "privkey.pem");
    cout << "carregou a certificacao SSL e os certificados\n";
	server_fd = soquete.cria_s(banco);
	cout << "criou servidor socket\n";

	while(1){
				
		cout << "Aguardando conexao de clientes... \n";
		
		cliente_fd = accept(server_fd,(struct sockaddr *)&cliente, &clientelen);
		
		if (cliente_fd < 0) cerr <<"Não pode aceitar:";
		id_process = fork();
		if (id_process < 0) cerr <<"ERRO em fork()";
		if (id_process ==0){
			cout << "Conexao: " << inet_ntoa(cliente.sin_addr) <<" : "<<ntohs(cliente.sin_port) << endl;
			ssl = SSL_new(ctx);
			SSL_set_fd(ssl, cliente_fd); 
			soquete.conversa_s(banco, ssl);
			exit (0);
		}
		else close(cliente_fd);
	}
	cout << "Passou while\n";
	close(server_fd);          /* close server socket */
    SSL_CTX_free(ctx); 
	
	soquete.encerra_s();
}
