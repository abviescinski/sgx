#include "menu.hpp"

menu_um::menu_um(){

};

void menu_um::criar_conta(SSL* ssl){
	int tipo = 0;
	bzero(opcao,LEN);
	bzero(buffer_cliente,LEN);
	
	//envia a opcao escolhida para o servidor
	strcpy(opcao, "Criar \0");
	SSL_write(ssl,opcao, strlen(opcao));   // encrypt & send message 
	
	//Recebe a resposta do servidor
	if((resposta_servidor = SSL_read(ssl, buffer_cliente, LEN)) > 0){
		buffer_cliente[resposta_servidor] = '\0';
		cout<< buffer_cliente<< endl;
    }
     
    //Solicita informações ao cliente
    if (strcmp(buffer_cliente, "Voce selecionou 'Criar conta'.") == 0){
		cout << "Entre com seu nome: ";
		cin.getline (cliente[0],LEN);
		
		cout << "CPF: ";
		cin.getline (cliente[1],LEN);
			
		cout << "Numero do telefone: ";
		cin.getline (cliente[2],LEN);
		
		cout << "Senha para conta: ";
		cin.getline (cliente[3],LEN);
		
		cout << endl << "Qual a finalidade da conta? \n"<< "	1 - Salario\n";
		cout << "	2 - Poupanca\n"<< "	3 - Corrente\n" << "Digite o numero correspondente: ";
		cin >> tipo;
		
		switch (tipo){
			case 1:
				strcpy(cliente[4], "Salario");
				break;
			case 2:
				strcpy(cliente[4], "Poupanca");
				break;
			case 3:
				strcpy(cliente[4], "Corrente");
				break;
			default:
				cout << "Opcao incorreta. Encerrando serviço.\n";
		}	
		for(int i = 0; i < 5; i++){
			SSL_write(ssl,cliente[i], LEN);
		}
		bzero(buffer_cliente,LEN);

		if((resposta_servidor = SSL_read(ssl, buffer_cliente, LEN)) > 0){
			buffer_cliente[resposta_servidor] = '\0';
			cout<<"Conta criada com sucesso! \n Numero: " << buffer_cliente<< endl;
		}	
	}
};

void menu_um::remover(SSL* ssl){
	bzero(opcao,LEN);
	bzero(buffer_cliente,LEN);

	//envia a opcao escolhida para o servidor
	strcpy(opcao, "Remover \0");
	SSL_write(ssl,opcao, strlen(opcao));   // encrypt & send message
	
	//Recebe a resposta do servidor
	if((resposta_servidor = SSL_read(ssl, buffer_cliente, LEN)) > 0){
		buffer_cliente[resposta_servidor] = '\0';
		cout<< buffer_cliente<< endl;
    }
	
	if (strcmp(buffer_cliente, "Voce selecionou 'Remover conta'.") == 0){
		cout <<"Qual número da conta a ser removida? ";
		cin >> conta[1];
	}
};

void menu_um::depositar(SSL* ssl){
	bzero(opcao,LEN);
	bzero(buffer_cliente,LEN);
	//envia a opcao escolhida para o servidor
	strcpy(opcao, "Depositar \0");
	SSL_write(ssl,opcao, strlen(opcao));   // encrypt & send message
	
	//Recebe a resposta do servidor
	if((resposta_servidor = SSL_read(ssl, buffer_cliente, LEN)) > 0){
		buffer_cliente[resposta_servidor] = '\0';
		cout<< buffer_cliente<< endl;
    }
	
	if (strcmp(buffer_cliente, "Voce selecionou 'Depositar'.") == 0){
		cout << "Digite o número da conta beneficiada: ";
		cin.getline (conta[0],LEN);
		cout <<"Qual valor será depositado? ";
		cin.getline (conta[1],LEN);
		for(int i = 0; i < 2; i++){
			SSL_write(ssl,conta[i], LEN);
		}
		bzero(opcao,LEN);
		SSL_read(ssl, buffer_cliente, LEN);
		cout << "Nome do beneficiado: " << buffer_cliente << ". Digite sim para confirmar: ";
		cin.getline (opcao,4);
		SSL_write(ssl, opcao, strlen(opcao));
	}
};

void menu_um::acessar(SSL* ssl){
	bzero(opcao,LEN);
	bzero(conta,LEN);
	bzero(buffer_cliente,LEN);
	menu_dois tela_dois;
	//envia a opcao escolhida para o servidor
	strcpy(opcao, "Acessar \0");
	SSL_write(ssl,opcao, strlen(opcao));   // encrypt & send message
	
	//Recebe a resposta do servidor
	if((resposta_servidor = SSL_read(ssl, buffer_cliente, LEN)) > 0){
		buffer_cliente[resposta_servidor] = '\0';
		cout<< buffer_cliente<< endl;
    }
	
	if (strcmp(buffer_cliente, "Voce selecionou 'Acessar a conta'.") == 0){
		cout <<"Informe o numero da sua conta: ";
		cin.getline (conta[0],LEN);
		cout <<"Senha: ";
		cin.getline (conta[1],LEN);
		for(int i = 0; i < 2; i++){
			SSL_write(ssl,conta[i], LEN);
		}
		
		resposta_servidor = 0;
		bzero(buffer_cliente,LEN);
		if((resposta_servidor = SSL_read(ssl, buffer_cliente, LEN)) > 0){
			tela_dois.opcoes(ssl, buffer_cliente);
		}else{
			cout <<"Não foi possivel acessar a conta, tente novamente!\n";
		}
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
menu_dois::menu_dois(){
	
};

void menu_dois::opcoes(SSL* ssl, char nome_cliente[LEN]){
	tipo = 0;
	cout << "\033c";
	cout <<"\n Seja bem vindo(a) "<< nome_cliente << "! \n";
	cout <<"	1 - Ver saldo.\n";
	cout <<"	2 - Realizar saque.\n";
	cout <<"	3 - Realizar transferencia.\n";
	cout <<"	0 - Sair e encerrar conexao.\n";
	cout <<"Selecione a opção desejada: ";
	cin >> tipo;
	
	cin.ignore();
		
	switch (tipo){
		case 1:
			strcpy(opcao, "Saldo\0");
			SSL_write(ssl,opcao, strlen(opcao));
			obter_saldo(ssl, nome_cliente);
			break;
		case 2:
			strcpy(opcao, "Saque\0");
			SSL_write(ssl,opcao, strlen(opcao));
			sacar(ssl, nome_cliente);
			break;
		case 3:
			strcpy(opcao, "Transferencia\0");
			SSL_write(ssl,opcao, strlen(opcao));
			transferir(ssl, nome_cliente);
			break;
		case 0:
			strcpy(opcao, "Sair\0");
			SSL_write(ssl,opcao, strlen(opcao));
			break;
		default:
			cout <<"Default."<<endl;
	}
};

void menu_dois::obter_saldo(SSL* ssl, char nome_cliente[LEN]){
	bzero(buffer_cliente,LEN);
	resposta_servidor = 0;
	tipo=0;
	//Recebe a resposta do servidor
	if((resposta_servidor = SSL_read(ssl, buffer_cliente, LEN)) > 0){
		buffer_cliente[resposta_servidor] = '\0';
		cout<< buffer_cliente<< endl;
    }
	
	bzero(buffer_cliente,LEN);
	resposta_servidor = 0;
	if((resposta_servidor = SSL_read(ssl, buffer_cliente, LEN)) > 0){
		buffer_cliente[resposta_servidor] = '\0';
		cout<< "O saldo atual é R$" << buffer_cliente<< endl;
		cout<< "O que deseja fazer agora? \n";
		cout<< "	1 - Voltar ao menu anterior.\n";
		cout<< "	2 - Encerrar conexao.\n";
		cin >> tipo;
		cin.ignore();
		switch (tipo){
			case 1: 
				opcoes(ssl, nome_cliente);
			case 2:
				cout << "Até mais! \n";
		}
    }
};

void menu_dois::sacar(SSL* ssl, char nome_cliente[LEN]){
	bzero(buffer_cliente,LEN);
	resposta_servidor = 0;
	//Recebe a resposta do servidor
	if((resposta_servidor = SSL_read(ssl, buffer_cliente, LEN)) > 0){
		buffer_cliente[resposta_servidor] = '\0';
		cout<< buffer_cliente<< endl;
    }
    
    bzero(buffer_cliente,LEN);
	resposta_servidor = 0;
	if((resposta_servidor = SSL_read(ssl, buffer_cliente, LEN)) > 0){
		buffer_cliente[resposta_servidor] = '\0';
		cout<< "O saldo atual é R$" << buffer_cliente<< endl;
    }
    cin.ignore();
    cout << "Digite o valor desejado: ";
	cin.getline (conta[0],LEN);
	SSL_write(ssl,conta[0], LEN);
	
	bzero(buffer_cliente,LEN);
	resposta_servidor = 0;
	if((resposta_servidor = SSL_read(ssl, buffer_cliente, LEN)) > 0){
		cout<<"Valor retirado com sucesso!"<< endl;
		cout<<"Seu saldo atualizado e' R$"<< buffer_cliente << ".\n";
		cout<< "O que deseja fazer agora? \n";
		cout<< "	1 - Voltar ao menu anterior.\n";
		cout<< "	2 - Encerrar conexao.\n";
		cin >> tipo;
		cin.ignore();
		switch (tipo){
			case 1: 
				opcoes(ssl, nome_cliente);
			case 2:
				cout << "Até mais! \n";
		}
    }else{
		cout<< "Erro ao finalizar saque, tente novamente! \n";
	}
};

void menu_dois::transferir(SSL* ssl, char nome_cliente[LEN]){
	bzero(buffer_cliente,LEN);
	resposta_servidor = 0;
	//Recebe a resposta do servidor
	if((resposta_servidor = SSL_read(ssl, buffer_cliente, LEN)) > 0){
		buffer_cliente[resposta_servidor] = '\0';
		cout<< buffer_cliente<< endl;
    }
    
    bzero(buffer_cliente,LEN);
	resposta_servidor = 0;
	if((resposta_servidor = SSL_read(ssl, buffer_cliente, LEN)) > 0){
		buffer_cliente[resposta_servidor] = '\0';
		cout<< "O saldo atual é R$"<< buffer_cliente<< endl;
    }
    
    cin.ignore();
    cout << "Digite o número da conta beneficiada: ";
	cin.getline (conta[0],LEN);
	cout <<"Qual valor será transferido? ";
	cin.getline (conta[1],LEN);
	
	for(int i = 0; i < 2; i++){
		SSL_write(ssl, conta[i], LEN);
	}
	
	bzero(opcao,LEN);
	SSL_read(ssl, buffer_cliente, LEN);
	cout << "Nome do beneficiado: " << buffer_cliente << ". Digite sim para finalizar a transferencia: ";
	cin.getline (opcao,LEN);
	SSL_write(ssl,opcao, LEN);
	
	if((resposta_servidor = SSL_read(ssl, buffer_cliente, LEN)) > 0){
		buffer_cliente[resposta_servidor] = '\0';
		cout<< "Seu saldo atualizado é R$"<< buffer_cliente<< endl;
		cout<< "O que deseja fazer agora? \n";
		cout<< "	1 - Voltar ao menu anterior.\n";
		cout<< "	2 - Encerrar conexao.\n";
		cin >> tipo;
		cin.ignore();
		switch (tipo){
			case 1: 
				opcoes(ssl, nome_cliente);
			case 2:
				cout << "Até mais! \n";
		}
    }else{
		cout<< "Erro ao finalizar transferencia, tente novamente! \n";
	}
};

void menu_dois::extrato(SSL* ssl, char nome_cliente[LEN]){
	bzero(buffer_cliente,LEN);
	resposta_servidor = 0;
	//Recebe a resposta do servidor
	if((resposta_servidor = SSL_read(ssl, buffer_cliente, LEN)) > 0){
		buffer_cliente[resposta_servidor] = '\0';
		cout<< buffer_cliente<< endl;
    }
    
    	cout<< "O que deseja fazer agora? \n";
		cout<< "	1 - Voltar ao menu anterior.\n";
		cout<< "	2 - Encerrar conexao.\n";
		cin >> tipo;
		cin.ignore();
		switch (tipo){
			case 1: 
				opcoes(ssl, nome_cliente);
			case 2:
				cout << "Até mais! \n";
		}
};
