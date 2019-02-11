#include "menu.hpp"
 
int main(int argc, char **argv) 
{
	cout << "** INICIO **\n";
    if ( argc != 2 )
    {
        cout << "Usar: "<< argv[0] << " <hostname>\n";
        exit(0);
    }
	
	socket_cliente socket;
	menu_um   tela_um;
	int opc, canal, resp;
	SSL_CTX *ctx;
	SSL *ssl;
	char buf_cliente[LEN];//Buffer
	char buf[LEN];//Buffer
	
	SSL_library_init();
cout << " Iniciou a biblioteca SSL\n";
	ctx = socket.InitCTX();
    canal = socket.inicia_conexao(argc, argv);
    ssl = SSL_new(ctx);      /* create new SSL connection state */
    SSL_set_fd(ssl, canal);    /* attach the socket descriptor */
cout << "210: criou cliente socket, cliente ssl.\n";

	if ( SSL_connect(ssl) == -1 ) {  /* perform the connection */
        cout << "A comunicação com o servidor falhou! :( \n";
        ERR_print_errors_fp(stderr);
	}
    else
    { 
		cout << "Solicitacao de conexao com servidor aceita\n";
		/*
		cout <<"Informe o numero da sua conta: ";
		cin.getline (buf_cliente,LEN);
		cout <<"Senha: ";
		cin.getline (buf,LEN);
		
		strcat (buf_cliente, " : ");
		strcat (buf_cliente,buf);
		
		cout << "Criou string para encaminhar ao servidor.\n";
		cout << "String: "<<buf_cliente <<endl;
		
		cout << "\nConectado com "<< SSL_get_cipher(ssl) << endl;
		
		socket.ShowCerts(ssl);        // get any certs 
cout << "Apresentou certificados e a conexao encriptada.\n";

        SSL_write(ssl,buf_cliente, strlen(buf_cliente));   // encrypt & send message 

cout << "Enviou mensagem ao servidor .\n";
		bzero(buf,LEN);
		resp = SSL_read(ssl, buf, sizeof(buf));
		buf[resp] = 0;
        cout << "Servidor disse: "<< buf<<endl;
        SSL_free(ssl); 
    }
	cout << "Finaliza conexoes .\n";
    close(canal);         // close socket
    SSL_CTX_free(ctx);	

	if (canal == 1){
		cout << "A comunicação com o servidor falhou! :( \n";
	}
	else
	{*/	
		cout << "\nConectado com "<< SSL_get_cipher(ssl) << endl;
		
		socket.ShowCerts(ssl);        // get any certs 
		cout << "Apresentou certificados e a conexao encriptada.\n";
		
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
				tela_um.criar_conta(ssl);
				break;
			case 2:
				tela_um.remover(ssl);
				break;
			case 3:
				tela_um.acessar(ssl);
				break;
			case 4:
				tela_um.depositar(ssl);
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
	}
	//colocar em loop para a pessoa selecionar as opcoes informadas ou encerrar o programa
	
}
