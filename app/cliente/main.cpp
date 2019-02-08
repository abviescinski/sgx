#include "menu.hpp"

int main(int argc, char **argv) 
{
	if (argc!=2)
    {
        printf("Use: %s <hostname>\n", argv[0]);
        exit(0);
    }
    
	
	SSL_CTX *ctx;
	SSL *ssl;
	socket_cliente socket;
	menu_um   tela_um;
	int opc, canal;
	
	char buffer[LEN];//Buffer
	int resp_servidor;
		
	SSL_library_init();
	ctx = socket.InitCTX();
	canal = socket.inicia_conexao(argc, argv);

	ssl = SSL_new(ctx); // criar novo estado de conexão SSL
	SSL_set_fd(ssl, canal); // anexar o descritor de soquete
	 
	if (SSL_connect(ssl) == -1 ){
		cout << "A comunicação com o servidor falhou! :( \n";
		ERR_print_errors_fp(stderr);
	}
	else
	{	
		socket.ShowCerts(ssl); // pegue qualquer certificado
		cout << "Conectado com encriptacao " << SSL_get_cipher(ssl);
				
		//Recebe a resposta do servidor
		//if((resp_servidor = SSL_read(ssl, buffer, LEN)) > 0){
			//buffer[resp_servidor] = '\0';
			SSL_read(ssl, buffer, sizeof(buffer));
			cout<<"servidor diz: " << buffer << endl;
		//}
		
		bzero(buffer,LEN);
		
		strcpy(buffer, "Ola Servidor, eu sou o seu cliente.");

		SSL_write(ssl,buffer, sizeof (buffer));   // criptografar e enviar mensagem
		
		//SSL_free(ssl); // liberar estado de conexão
		
		//close(canal);
	}
		/*
		cout <<"\nSelecione uma opção:\n";
		cout <<"	1 - Criar conta.\n";
		cout <<"	2 - Remover conta.\n";
		cout <<"	3 - Acesso a conta.\n";
		cout <<"	4 - Depositar.\n";
		cout <<"	0 - Encerrar conexao.\n";
		cin >> opc;
		
		cin.ignore();
		
		switch (opc) {
			case 1:
				tela_um.criar_conta(canal);
				break;
			case 2:
				tela_um.remover(canal);
				break;
			case 3:
				tela_um.acessar(canal);
				break;
			case 4:
				tela_um.depositar(canal);
				break;
			case 0:
				cout<< "Ate mais! \n";
				socket.fecha_conexao();
				break;
			default:
				cout <<"Selecione uma opcao: \n";
				cin >> opc;
				cin.ignore();
		}
	}*/
	//colocar em loop para a pessoa selecionar as opcoes informadas ou encerrar o programa
		
}
