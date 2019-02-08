#include "menu.hpp"

menu_um::menu_um(){

};

void menu_um::criar_conta(int canal){
	bzero(opcao,LEN);
	bzero(buffer_client,LEN);
	
	//envia a opcao escolhida para o servidor
	strcpy(opcao, "Criar \0");
	send(canal, opcao, strlen(opcao), 0);
	
	//Recebe a resposta do servidor
	if((resposta_servidor = recv(canal, buffer_client, LEN, 0)) > 0){
		buffer_client[resposta_servidor] = '\0';
		cout<< buffer_client<< endl;
    }
     
    //Solicita informações ao cliente
    if (strcmp(buffer_client, "Voce selecionou 'Criar conta'.") == 0){
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
			send(canal, cliente[i], LEN, 0);
		}
		bzero(buffer_client,LEN);
		if((resposta_servidor = recv(canal, buffer_client, LEN, 0)) > 0){
			buffer_client[resposta_servidor] = '\0';
			cout<<"Conta criada com sucesso! \n Numero: " << buffer_client<< endl;
		}	
	}
};

void menu_um::remover(int canal){
	bzero(opcao,LEN);
	bzero(buffer_client,LEN);

	//envia a opcao escolhida para o servidor
	strcpy(opcao, "Remover \0");
	send(canal, opcao, strlen(opcao), 0);
	
	//Recebe a resposta do servidor
	if((resposta_servidor = recv(canal, buffer_client, LEN, 0)) > 0){
		buffer_client[resposta_servidor] = '\0';
		cout<< buffer_client<< endl;
    }
	
	if (strcmp(buffer_client, "Voce selecionou 'Remover conta'.") == 0){
		cout <<"Qual número da conta a ser removida? ";
		cin >> conta[1];
	}
};

void menu_um::acessar(int canal){
	bzero(opcao,LEN);
	bzero(buffer_client,LEN);
	bzero(conta,LEN);
	menu_dois tela_dois;
	
	//envia a opcao escolhida para o servidor
	strcpy(opcao, "Acessar \0");
	send(canal, opcao, strlen(opcao), 0);
	//recebe a resposta (confirmacao de que entrou na mesma funcao)
	if((resposta_servidor = recv(canal, buffer_client, LEN, 0)) > 0){
		buffer_client[resposta_servidor] = '\0';
		cout<< buffer_client<< endl;
    }
	
	if (strcmp(buffer_client, "Voce selecionou 'Acessar a conta'.") == 0){
		cout <<"Informe o numero da sua conta: ";
		cin.getline (conta[0],LEN);
		cout <<"Senha: ";
		cin.getline (conta[1],LEN);
		for(int i = 0; i < 2; i++){
			send(canal, conta[i], LEN, 0);
		}
		tipo = 0;
		resposta_servidor = 0;
		bzero(buffer_client,LEN);
		
		if((resposta_servidor = recv(canal, buffer_client, LEN, 0)) > 0){
			cout << endl << endl; // depois colocar uma limpeza de tela aqui
			cout <<"Seja bem vindo(a) "<< buffer_client << "! \n";
			cout <<"Selecione a opção desejada: \n";
			cout <<"	1 - Ver saldo.\n";
			cout <<"	2 - Realizar saque.\n";
			cout <<"	3 - Realizar transferencia.\n";
			cout <<"	0 - Sair e encerrar conexao.\n";
			cin >> tipo;
			
			switch (tipo){
				case 1:
					strcpy(opcao, "Saldo\0");
					send(canal, opcao, strlen(opcao), 0);
					tela_dois.obter_saldo(canal);
					break;
				case 2:
					strcpy(opcao, "Saque\0");
					send(canal, opcao, strlen(opcao), 0);
					tela_dois.sacar(canal);
					break;
				case 3:
					strcpy(opcao, "Transferencia\0");
					send(canal, opcao, strlen(opcao), 0);
					tela_dois.transferir(canal);
					break;
				case 0:
					strcpy(opcao, "Sair\0");
					send(canal, opcao, strlen(opcao), 0);
					break;
				default:
					cout <<"Default."<<endl;
			}
			
		}
	}
};

void menu_um::depositar(int canal){
	bzero(opcao,LEN);
	bzero(buffer_client,LEN);
	//envia a opcao escolhida para o servidor
	strcpy(opcao, "Depositar \0");
	send(canal, opcao, strlen(opcao), 0);
	
	if((resposta_servidor = recv(canal, buffer_client, LEN, 0)) > 0){
		buffer_client[resposta_servidor] = '\0';
		cout<< buffer_client<< endl;
    }
	
	if (strcmp(buffer_client, "Voce selecionou 'Depositar'.") == 0){
		cout << "Digite o número da conta beneficiada: ";
		cin.getline (conta[0],LEN);
		cout <<"Qual valor será depositado? ";
		cin.getline (conta[1],LEN);
		for(int i = 0; i < 2; i++){
			cout << conta[i] << endl;
			send(canal, conta[i], LEN, 0);
		}
		bzero(opcao,LEN);
		recv(canal, buffer_client, LEN, 0);
		cout << "Nome do beneficiado: " << buffer_client << ". Digite sim para confirmar.\n";
		cin.getline (opcao,4);
		send(canal, opcao, 4, 0);
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
menu_dois::menu_dois(){
	
};

void menu_dois::obter_saldo(int canal){
	bzero(buffer_client,LEN);
	resposta_servidor = 0;
	if((resposta_servidor = recv(canal, buffer_client, LEN, 0)) > 0){
		buffer_client[resposta_servidor] = '\0';
		cout<< buffer_client<< endl;
    }
	
	bzero(buffer_client,LEN);
	resposta_servidor = 0;
	if((resposta_servidor = recv(canal, buffer_client, LEN, 0)) > 0){
		buffer_client[resposta_servidor] = '\0';
		cout<< "O saldo atual é R$" << buffer_client<< endl;
    }
};

void menu_dois::sacar(int canal){
	bzero(buffer_client,LEN);
	resposta_servidor = 0;
	if((resposta_servidor = recv(canal, buffer_client, LEN, 0)) > 0){
		buffer_client[resposta_servidor] = '\0';
		cout<< buffer_client<< endl;
    }
    
    bzero(buffer_client,LEN);
	resposta_servidor = 0;
	if((resposta_servidor = recv(canal, buffer_client, LEN, 0)) > 0){
		buffer_client[resposta_servidor] = '\0';
		cout<< "O saldo atual é R$" << buffer_client<< endl;
    }
    cin.ignore();
    cout << "Digite o valor desejado: ";
	cin.getline (conta[0],LEN);
	send(canal, conta[0], LEN, 0);
	
	bzero(buffer_client,LEN);
	resposta_servidor = 0;
	if((resposta_servidor = recv(canal, buffer_client, LEN, 0)) > 0){
		cout<<"Valor retirado com sucesso!"<< endl;
		cout<<"Seu saldo agora e' R$"<< buffer_client << "."<<endl;
    }
};

void menu_dois::transferir(int canal){
	bzero(buffer_client,LEN);
	resposta_servidor = 0;
	if((resposta_servidor = recv(canal, buffer_client, LEN, 0)) > 0){
		buffer_client[resposta_servidor] = '\0';
		cout<< buffer_client<< endl;
    }
    
    bzero(buffer_client,LEN);
	resposta_servidor = 0;
	if((resposta_servidor = recv(canal, buffer_client, LEN, 0)) > 0){
		buffer_client[resposta_servidor] = '\0';
		cout<< "O saldo atual é R$"<< buffer_client<< endl;
    }
    
    cin.ignore();
    cout << "Digite o número da conta beneficiada: ";
	cin.getline (conta[0],LEN);
	cout <<"Qual valor será depositado? ";
	cin.getline (conta[1],LEN);
	
	for(int i = 0; i < 2; i++){
		send(canal, conta[i], LEN, 0);
	}
	
	bzero(opcao,LEN);
	recv(canal, buffer_client, LEN, 0);
	cout << "Nome do beneficiado: " << buffer_client << ". Digite sim para finalizar a transferencia. "<< endl;
	cin.getline (opcao,LEN);
	send(canal, opcao, LEN, 0);
	
	if((resposta_servidor = recv(canal, buffer_client, LEN, 0)) > 0){
		buffer_client[resposta_servidor] = '\0';
		cout<< "Seu saldo atual é R$"<< buffer_client<< endl;
    }
};

void menu_dois::extrato(int canal){
	bzero(buffer_client,LEN);
	resposta_servidor = 0;
	if((resposta_servidor = recv(canal, buffer_client, LEN, 0)) > 0){
		buffer_client[resposta_servidor] = '\0';
		cout<< buffer_client<< endl;
    }
};
