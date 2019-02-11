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

		cout << "\nConectado com "<< SSL_get_cipher(ssl) << endl;
		
		socket.ShowCerts(ssl);        // get any certs 
		cout << "Apresentou certificados e a conexao encriptada.\n";
		cout << "\033c";
		
		cout <<"\n**Olá Cliente!**\n";
		cout <<"	1 - Criar conta.\n";
		cout <<"	2 - Remover conta.\n";
		cout <<"	3 - Acesso a conta.\n";
		cout <<"	4 - Depositar.\n";
		cout <<"	0 - Encerrar conexao.\n";
		cout <<"Selecione uma opção: ";
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
				cout <<"Opção incorreta, encerrando conexão.\n";
				socket.fecha_conexao();
		}
	}
	//colocar em loop para a pessoa selecionar as opcoes informadas ou encerrar o programa
	
}
