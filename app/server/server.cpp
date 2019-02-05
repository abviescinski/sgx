#include "server.hpp"

socket_server::socket_server(){
	
	
}

MYSQL socket_server::conecta_bd(){

	MYSQL conexao;

    mysql_init(&conexao);
    if ( mysql_real_connect(&conexao, HOST, USER, PASS, DB, 0, NULL, 0) )
    {
          printf("Conectado na base de dados com sucesso!\n");
          return conexao;
    }
    else
    {
          printf("Falha de conexao\n");
          printf("Erro %d : %s\n", mysql_errno(&conexao), mysql_error(&conexao));
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int socket_server::cria_s(){

    /* Cria um soquete IPv4 */
    this->serverfd = socket(AF_INET, SOCK_STREAM, 0);//ok
    if(this->serverfd < 0) { //ok
        perror("Nao pode criar o servidor socket: ");
        return EXIT_FAILURE;
    }
    fprintf(stdout, "Servidor soquete criado com fd: %d\n", this->serverfd);

    /* Define as propriedades do soquete do servidor */
    server.sin_family = AF_INET;//ok
    server.sin_port = htons(PORT);
    memset(server.sin_zero, 0x0, 8);

    /* Lidar com o erro da porta já em uso */
    int yes = 1;
    if(setsockopt(this->serverfd, SOL_SOCKET, SO_REUSEADDR,
                  &yes, sizeof(int)) == -1) {
        perror("Erro de opcoes de soquete:");
        return EXIT_FAILURE;
    }

    /* ligar o soquete a uma porta */
    if(bind(this->serverfd, (struct sockaddr*)&server, sizeof(server)) < 0 ) {
        perror("Erro de ligacao do soquete:");
        return EXIT_FAILURE;
    }
    
    //MUDA A PARTIR DAQUI
    
    /* Começa a esperar conexões de clientes, +/- ok */
    if(listen(this->serverfd, 1) == -1) {
        perror("Erro de escuta:");
    }
    fprintf(stdout, "Escutando na porta %d\n", PORT);

    socklen_t client_len = sizeof(client);
    if ((this->clientfd=accept(this->serverfd,
        (struct sockaddr *) &client, &client_len )) == -1) {
        perror("Não pode aceitar:");
    }
    
    return clientfd;
}

void socket_server::conecta_s(MYSQL banco, int clientfd){

    /* Cópias em buffer nossa mensagem de boas vindas */
    strcpy(buffer_serv, "****Inicio Cliente****\0");

    /* Envia a mensagem para o cliente */
	if (send(clientfd, buffer_serv, strlen(buffer_serv), 0)) {
        printf( "Cliente conectado, aguardando mensagem.\n");
		menu_um tela_um;
		
        // Zerando Buffers 
        memset(buffer_serv, 0x0, BUFFER_LENGTH);

        // Recebe mensagem do cliente 
        int message_len;
        if((message_len = recv(clientfd, buffer_serv, BUFFER_LENGTH, 0)) > 0){
			buffer_serv[message_len - 1] = '\0';
            printf("Cliente disse:%s\n", buffer_serv);
        }
        
        if(strcmp(buffer_serv, "Criar") == 0){
			tela_um.criar_conta(clientfd, banco);
        }else{
            if (strcmp(buffer_serv, "Remover") == 0){
				tela_um.remover(clientfd, banco);					
			}else{
				if (strcmp(buffer_serv, "Acessar") == 0){
					tela_um.acessar(clientfd, banco);					
				}else{
					if (strcmp(buffer_serv, "Depositar") == 0){
						tela_um.depositar(clientfd, banco);					
					}else{
						if (strcmp(buffer_serv, "bye") == 0){
							//Conexão do cliente Fechar 
							close(clientfd);				
						}
					}
				}
			}
        }
        
	}
     
}

void socket_server::encerra_s(){

    /* Feche o soquete local */
    close(this->serverfd);

    cout << "Conexao encerrada"<< endl;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
menu_um::menu_um(){
	
};

void menu_um::criar_conta(int client_fd, MYSQL banco){
	send(client_fd, "Voce selecionou 'Criar conta'.", BUFFER_LENGTH, 0);
		
	for(int i = 0; i < 5; i++){
		if((contador = recv(client_fd, dados_cliente[i], BUFFER_LENGTH, 0)) > 0){
			dados_cliente[i][contador - 1] = '\0';
			printf("Cliente disse:%s\n", dados_cliente[i]);
		}
	}
	
	strcpy(str_banco,"INSERT INTO cliente(nome, cpf, telefone) values('");
	strcat(str_banco, dados_cliente[0]);
	strcat(str_banco, "','");
	strcat(str_banco, dados_cliente[1]);
	strcat(str_banco, "','");
	strcat(str_banco, dados_cliente[2]);
	strcat(str_banco, "');");
	
	res_banco = mysql_query(&banco,str_banco);
	
	if (!res_banco) printf("Registros inseridos %d\n", mysql_affected_rows(&banco));
    else printf("Erro na inserção %d : %s\n", mysql_errno(&banco), mysql_error(&banco));
     
    memset(str_banco, 0x0, BUFFER_LENGTH);
    
	strcpy(str_banco, "SELECT cliente.ID_Cliente FROM cliente WHERE nome = '");
	strcat(str_banco,dados_cliente[0]);
	strcat(str_banco,"';");

	if (mysql_query(&banco,str_banco)){
		printf("Erro: %s\n",mysql_error(&banco));
	}else{
		res_consulta = mysql_store_result(&banco);//recebe a consulta
		if (res_consulta) //se houver consulta
        {
			dados_banco=mysql_fetch_row(res_consulta);
		}
	}
	
	memset(str_banco, 0x0, BUFFER_LENGTH);
	
    strcpy(str_banco,"INSERT INTO conta(senha, saldo, tipo, C_ID_Cliente) values('");
	strcat(str_banco, dados_cliente[3]);
	strcat(str_banco, "','100','");
	strcat(str_banco, dados_cliente[4]);
	strcat(str_banco, "','");
	strcat(str_banco, dados_banco[0]);
	strcat(str_banco, "');");
	
	res_banco = mysql_query(&banco,str_banco);
	
	if (!res_banco){
		printf("Registros inseridos %d\n", mysql_affected_rows(&banco));
		memset(str_banco, 0x0, BUFFER_LENGTH);
		strcpy(str_banco, "SELECT conta.ID_Conta FROM conta INNER JOIN cliente ON cliente.ID_Cliente = ");
		strcat(str_banco,dados_banco[0]);
		strcat(str_banco," AND conta.C_ID_Cliente = cliente.ID_Cliente;");

		memset(dados_banco, 0x0, BUFFER_LENGTH);
		if (mysql_query(&banco,str_banco)){
			printf("Erro: %s\n",mysql_error(&banco));
		}else{
			res_consulta = mysql_store_result(&banco);//recebe a consulta
			if (res_consulta) //se houver consulta
			{
				dados_banco=mysql_fetch_row(res_consulta);
				send(client_fd, dados_banco[0], BUFFER_LENGTH, 0);
			}
		}
    }else{
		printf("Erro na inserção %d : %s\n", mysql_errno(&banco), mysql_error(&banco));
    }
};

void menu_um::remover(int client_fd, MYSQL banco){
	send(client_fd, "Voce selecionou 'Remover conta'.", BUFFER_LENGTH, 0);

};

void menu_um::acessar(int client_fd, MYSQL banco){
	send(client_fd, "Voce selecionou 'Acessar a conta'.", BUFFER_LENGTH, 0);
	
	memset(str_banco, 0x0, BUFFER_LENGTH);
	memset(buffer_respostas, 0x0, BUFFER_LENGTH);
	menu_dois tela_dois;
	
	for (int i = 0; i < 2; i++){
		if((contador = recv(client_fd, dados_cliente[i], BUFFER_LENGTH, 0)) > 0){
			dados_cliente[i][contador - 1] = '\0';
			printf("Cliente disse:%s\n", dados_cliente[i]);
		}
	}
	
	//descobre o nome do cliente que esta fazendo login com base no id da conta e da senha
	strcpy(str_banco, "SELECT cliente.nome FROM conta INNER JOIN cliente ON conta.ID_conta = ");
	strcat(str_banco,dados_cliente[0]);
	strcat(str_banco," AND conta.senha = ");
	strcat(str_banco,dados_cliente[1]);
	strcat(str_banco," AND conta.C_ID_Cliente = cliente.ID_Cliente;");

	if (mysql_query(&banco,str_banco)){
		printf("Erro: %s\n",mysql_error(&banco));
	}else{
		//se caso nao deu erro na consulta, independente de ter retornado positivo ou negativo, entra aqui
		res_consulta = mysql_store_result(&banco);//recebe a consulta
		if ((dados_banco=mysql_fetch_row(res_consulta)) != NULL){
			contador = 0;
			strcpy(dados_cliente[2], dados_banco[0]);
            send(client_fd, dados_banco[0], BUFFER_LENGTH, 0);
            //criar outra clase para esses codigos IF pois sao o menu 2
			if((contador = recv(client_fd, buffer_respostas, BUFFER_LENGTH, 0)) > 0){
				if(strcmp(buffer_respostas, "Saldo") == 0){
					printf("Acesso -> Saldo \n");
					tela_dois.obtem_saldo(client_fd, banco, dados_cliente);
				}else{
					if (strcmp(buffer_respostas, "Saque") == 0){
						printf("Acesso -> Saque \n");
						tela_dois.sacar(client_fd, banco, dados_cliente);					
					}else{
						if (strcmp(buffer_respostas, "Transferencia") == 0){
							printf("Acesso -> Transferencia \n");
							tela_dois.transferir(client_fd, banco, dados_cliente);					
						}else{
							if (strcmp(buffer_respostas, "Sair") == 0){
								printf("Acesso -> Sair \n");					
							}
						}
					}
				}
				
			}
		}
    }
};

void menu_um::depositar(int client_fd, MYSQL banco){
	send(client_fd, "Voce selecionou 'Depositar'.", BUFFER_LENGTH, 0);
	for (int i = 0; i < 2; i++){
		if((contador = recv(client_fd, dados_cliente[i], BUFFER_LENGTH, 0)) > 0){
		 dados_cliente[i][contador - 1] = '\0';
         printf("Cliente disse:%s\n", dados_cliente[i]);
		}
	}
	
	//monta a string de selecao ja somando o valor a ser depositado
	strcpy(str_banco, "SELECT cliente.nome FROM conta INNER JOIN cliente ON conta.ID_conta = ");
	strcat(str_banco,dados_cliente[0]);
	strcat(str_banco," AND conta.C_ID_Cliente = cliente.ID_Cliente;");
	
	//realiza a busca no banco de dados, trata erro ou confirma e armazena os resultados em *dados_banco
	// onde linha[0] e' o saldo na conta+ o valor a ser depositado e linha[1] e' o nome do beneficiado
	if (mysql_query(&banco,str_banco)){
		printf("Erro: %s\n",mysql_error(&banco));
	}else{
		res_consulta = mysql_store_result(&banco);//recebe a consulta
		if (res_consulta) //se houver consulta
        {
			dados_banco=mysql_fetch_row(res_consulta);
			send(client_fd, dados_banco[0], BUFFER_LENGTH, 0);
		}
	}
	
	//recebe confirmacao de insercao
	recv(client_fd, buffer_respostas, BUFFER_LENGTH, 0);
	
	if (strcmp(buffer_respostas, "sim") == 0){
		memset(str_banco, 0x0, BUFFER_LENGTH);		
		strcpy(str_banco, "UPDATE `conta` SET `saldo` = saldo + ");
		strcat(str_banco, dados_cliente[1]);
		strcat(str_banco, " WHERE `ID_Conta` = ");
		strcat(str_banco, dados_cliente[0]);
		strcat(str_banco, ";");
		res_banco = mysql_query(&banco,str_banco);
		if (!res_banco) printf("Registros alterados %d\n", mysql_affected_rows(&banco));
		else printf("Erro na inserção %d : %s\n", mysql_errno(&banco), mysql_error(&banco));
	}
	
	//falta tratar erro quando o numero da conta não existe
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
menu_dois::menu_dois(){
	
};

void menu_dois::obtem_saldo(int client_fd, MYSQL banco, char dados_acesso[3][BUFFER_LENGTH]){
	send(client_fd, "Você selecionou 'Saldo'.", BUFFER_LENGTH, 0);
	
	//monta a string de selecao para descobrir o saldo e envia ao cliente
	memset(str_banco, 0x0, BUFFER_LENGTH);
	strcpy(str_banco, "SELECT conta.saldo FROM conta WHERE conta.ID_conta = ");
	strcat(str_banco, dados_acesso[0]);
	strcat(str_banco, ";");

	if (mysql_query(&banco,str_banco)){
		printf("Erro: %s\n",mysql_error(&banco));
	}else{
		res_consulta = mysql_store_result(&banco);//recebe a consulta
		if (res_consulta) //se houver consulta
        {
			dados_banco=mysql_fetch_row(res_consulta);
			printf("Saldo:%s\n", dados_banco[0]);
			send(client_fd, dados_banco[0], BUFFER_LENGTH, 0);
		}
	}
	
};

void menu_dois::sacar(int client_fd, MYSQL banco, char dados_acesso[3][BUFFER_LENGTH]){
	send(client_fd, "Você selecionou 'Sacar'.", BUFFER_LENGTH, 0);
	
	//apresentar o saldo primeiro e depois perguntar qual valor deseja transferir
	//monta a string de selecao para descobrir o saldo e envia ao cliente
	memset(str_banco, 0x0, BUFFER_LENGTH);
	strcpy(str_banco, "SELECT conta.saldo FROM conta WHERE conta.ID_conta = ");
	strcat(str_banco, dados_acesso[0]);
	strcat(str_banco, ";");

	if (mysql_query(&banco,str_banco)){
		printf("Erro: %s\n",mysql_error(&banco));
	}else{
		res_consulta = mysql_store_result(&banco);//recebe a consulta
		if (res_consulta) //se houver consulta
        {
			dados_banco=mysql_fetch_row(res_consulta);
			send(client_fd, dados_banco[0], BUFFER_LENGTH, 0);
		}
	}
	
	if((contador = recv(client_fd, dados_cliente, BUFFER_LENGTH, 0)) > 0){
        printf("Cliente disse:%s\n", dados_cliente[0]);
		
		memset(str_banco, 0x0, BUFFER_LENGTH);		
		strcpy(str_banco, "UPDATE `conta` SET `saldo` = saldo - ");
		strcat(str_banco, dados_cliente[0]);
		strcat(str_banco, " WHERE `ID_Conta` = ");
		strcat(str_banco, dados_acesso[0]);
		strcat(str_banco, ";");
		res_banco = mysql_query(&banco,str_banco);
		if (!res_banco){
			printf("Registros alterados %d\n", mysql_affected_rows(&banco));
			//apresentar o saldo primeiro e depois perguntar qual valor deseja transferir
			//monta a string de selecao para descobrir o saldo e envia ao cliente
			memset(str_banco, 0x0, BUFFER_LENGTH);
			strcpy(str_banco, "SELECT conta.saldo FROM conta WHERE conta.ID_conta = ");
			strcat(str_banco, dados_acesso[0]);
			strcat(str_banco, ";");

			if (mysql_query(&banco,str_banco)){
				printf("Erro: %s\n",mysql_error(&banco));
			}else{
				res_consulta = mysql_store_result(&banco);//recebe a consulta
				if (res_consulta) //se houver consulta
				{
					dados_banco=mysql_fetch_row(res_consulta);
					send(client_fd, dados_banco[0], BUFFER_LENGTH, 0);
				}
			}
		}else{
			printf("Erro na inserção %d : %s\n", mysql_errno(&banco), mysql_error(&banco));
		}
	}
	
};

void menu_dois::transferir(int client_fd, MYSQL banco, char dados_acesso[3][BUFFER_LENGTH]){
	send(client_fd, "Você selecionou 'Transferir'.", BUFFER_LENGTH, 0);
	
	//apresentar o saldo primeiro e depois perguntar qual valor deseja transferir
	//monta a string de selecao para descobrir o saldo e envia ao cliente
	memset(str_banco, 0x0, BUFFER_LENGTH);
	strcpy(str_banco, "SELECT conta.saldo FROM conta WHERE conta.ID_conta = ");
	strcat(str_banco, dados_acesso[0]);
	strcat(str_banco, ";");

	if (mysql_query(&banco,str_banco)){
		printf("Erro: %s\n",mysql_error(&banco));
	}else{
		res_consulta = mysql_store_result(&banco);//recebe a consulta
		if (res_consulta) //se houver consulta
        {
			dados_banco=mysql_fetch_row(res_consulta);
			send(client_fd, dados_banco[0], BUFFER_LENGTH, 0);
		}
	}
	
	//recebe o valor e a conta que sera beneficiada
	for (int i = 0; i < 2; i++){
		if((contador = recv(client_fd, dados_cliente[i], BUFFER_LENGTH, 0)) > 0){
		 dados_cliente[i][contador - 1] = '\0';
         printf("Cliente disse:%s\n", dados_cliente[i]);
		}
	}
	
	//monta a string de selecao para descobrir o nome do beneficiado
	strcpy(str_banco, "SELECT cliente.nome FROM conta INNER JOIN cliente ON conta.ID_conta = ");
	strcat(str_banco,dados_cliente[0]);
	strcat(str_banco," AND conta.C_ID_Cliente = cliente.ID_Cliente;");
	
	//realiza a busca no banco de dados, trata erro ou confirma e armazena os resultados em *dados_banco
	// onde linha[0] e' o (saldo na conta+ o valor a ser depositado) e linha[1] e' o nome do beneficiado
	if (mysql_query(&banco,str_banco)){
		printf("Erro: %s\n",mysql_error(&banco));
	}else{
		res_consulta = mysql_store_result(&banco);//recebe a consulta
		if (res_consulta) //se houver consulta
        {
			dados_banco=mysql_fetch_row(res_consulta);
			send(client_fd, dados_banco[0], BUFFER_LENGTH, 0);
		}
	}
	
	//recebe confirmacao de insercao
	recv(client_fd, buffer_respostas, BUFFER_LENGTH, 0);
	
	if (strcmp(buffer_respostas, "sim") == 0){
		//insere valor na conta do beneficiado
		memset(str_banco, 0x0, BUFFER_LENGTH);		
		strcpy(str_banco, "UPDATE `conta` SET `saldo` = saldo + ");
		strcat(str_banco, dados_cliente[1]);
		strcat(str_banco, " WHERE `ID_Conta` = ");
		strcat(str_banco, dados_cliente[0]);
		strcat(str_banco, ";");
		res_banco = mysql_query(&banco,str_banco);
		if (!res_banco) printf("Registros alterados %d\n", mysql_affected_rows(&banco));
		else printf("Erro na inserção %d : %s\n", mysql_errno(&banco), mysql_error(&banco));
		
		//descontar o valor transferido da conta atual
		memset(str_banco, 0x0, BUFFER_LENGTH);		
		strcpy(str_banco, "UPDATE `conta` SET `saldo` = saldo - ");
		strcat(str_banco, dados_cliente[1]);
		strcat(str_banco, " WHERE `ID_Conta` = ");
		strcat(str_banco, dados_acesso[0]);
		strcat(str_banco, ";");
		res_banco = mysql_query(&banco,str_banco);
		if (!res_banco) printf("Registros alterados %d\n", mysql_affected_rows(&banco));
		else printf("Erro na inserção %d : %s\n", mysql_errno(&banco), mysql_error(&banco));
		
		//apresentar o saldo
		//monta a string de selecao para descobrir o saldo e envia ao cliente
		memset(str_banco, 0x0, BUFFER_LENGTH);
		strcpy(str_banco, "SELECT conta.saldo FROM conta WHERE conta.ID_conta = ");
		strcat(str_banco, dados_acesso[0]);
		strcat(str_banco, ";");

		if (mysql_query(&banco,str_banco)){
			printf("Erro: %s\n",mysql_error(&banco));
		}else{
			res_consulta = mysql_store_result(&banco);//recebe a consulta
			if (res_consulta) //se houver consulta
			{
				dados_banco=mysql_fetch_row(res_consulta);
				send(client_fd, dados_banco[0], BUFFER_LENGTH, 0);
			}
		}
	}
};

void menu_dois::extrato(int client_fd, MYSQL banco, char dados_acesso[3][BUFFER_LENGTH]){
	send(client_fd, "Você selecionou 'Extrato'.", BUFFER_LENGTH, 0);
};
