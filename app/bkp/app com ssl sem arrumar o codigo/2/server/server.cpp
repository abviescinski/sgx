#include "server.hpp"

socket_server::socket_server(){
	
	
}

MYSQL socket_server::conecta_bd()
{	MYSQL conexao;
    mysql_init(&conexao);
    if ( mysql_real_connect(&conexao, HOST, USER, PASS, DB, 0, NULL, 0) )
    {
          cout << "Conectado na base de dados com sucesso!\n";
          return conexao;
    }
    else
    {
          cout << "Falha de conexao\n";
          cerr << "Erro " << mysql_errno(&conexao) << " : " << mysql_error(&conexao);
    }

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SSL_CTX* socket_server::InitServerCTX(void)
{   const SSL_METHOD *method;
    SSL_CTX *ctx;
    
	cout << "Inicio server CTX, carregar as coisas do OpenSSL\n";
	
    OpenSSL_add_all_algorithms();  /* load & register all cryptos, etc. */
    SSL_load_error_strings();   /* load all error messages */
    method = TLS_server_method();  /* create new server-method instance */
    ctx = SSL_CTX_new(method);   /* create new context from method */
    
    cout<<"Finalizou os carregamentos SSL, verifica se deu tudo certo.\n";
    
    if ( ctx == NULL )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    
    cout << "Finaliza a função Init server e retorna ctx.\n";
    return ctx;
}

void socket_server::LoadCertificates(SSL_CTX* ctx, const char* CertFile, const char* KeyFile)
{   /* set the local certificate from CertFile */
    if ( SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    
    cout << "Load Certificates, verifica o pem do certificado se esta correto\n";
    
    /* set the private key from KeyFile (may be the same as CertFile) */
    if ( SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM) <= 0 )
    {
        ERR_print_errors_fp(stderr);
        abort();
    }
    
    cout << "Load Certificates, verifica o se pem do chave privada esta correto\n";
    
    /* verify private key */
    if ( !SSL_CTX_check_private_key(ctx) )
    {
        cout << "Private key does not match the public certificate\n";
        abort();
    }
    
    cout <<"Finaliza a função LoadCertificates, tudo certo ate aqui\n";
}

void socket_server::ShowCerts(SSL* ssl)
{   X509 *cert;
    char *line;
 
cout <<"116: Apresenta certificados \n";
 
    cert = SSL_get_peer_certificate(ssl); /* Get certificates (if available) */
    
    cout <<"120: Obter certificados (se disponíveis)\n";
    
    if ( cert != NULL )
    {
        cout <<"\n--------INFORMACOES DO CERTIFICADO--------\n";
        cout <<"Server certificates:\n";
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        cout <<"Subject: "<< line << endl;
        free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        cout << "Issuer: "<< line << endl;
        free(line);
        X509_free(cert);
    }
    else cout << "134: LADO SERVIDOR, sem certificados!\n";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int socket_server::cria_s(MYSQL banco){
    // Cria um soquete IPv4
    this->serverfd = socket(PF_INET, SOCK_STREAM, 0);
    if(this->serverfd < 0) {
        cerr << "Nao pode criar o servidor socket: ";
        return EXIT_FAILURE;
    }
    cout << "Servidor soquete criado com fd: " << this->serverfd << endl;
	
	bzero((char*)&server, sizeof(server));
	
    // Define as propriedades do soquete do servidor
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY; 
    server.sin_port = htons(PORT);
   
    // Lidar com o erro da porta já em uso
    int yes = 1;
    if(setsockopt(this->serverfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        cerr <<"Erro de opcoes de soquete:";
        return EXIT_FAILURE;
    }

    // ligar o soquete a uma porta
    if(bind(this->serverfd, (struct sockaddr*)&server, sizeof(server))<0){
        cerr <<"Erro de ligacao do soquete:";
        return EXIT_FAILURE;
    }
    
    // Começa a esperar conexões de clientes
    if(listen(this->serverfd, 5) == -1) {
        cerr<<"Erro de escuta:";
    }
    cout << "Escutando na porta: " << PORT << endl;

    return serverfd;
}

void socket_server::conversa_s(MYSQL banco, SSL* ssl){
	int message_len;
	menu_um tela_um;
	
	if ( SSL_accept(ssl) == -1 )     /* do SSL-protocol accept */
        ERR_print_errors_fp(stderr);
    else
    {
        ShowCerts(ssl); /* get any certificates */
		cout << "Cliente conectado, aguardando mensagem.\n";
       
		if((message_len = SSL_read(ssl, buffer_server, BUFFER_LENGTH)) > 0){
        //message_len = SSL_read(ssl, buffer_server, BUFFER_LENGTH); /* get request */
			buffer_server[message_len] = '\0';
			cout << "Cliente diz: "<< buffer_server << endl;
 
			if(strcmp(buffer_server, "Criar ") == 0){
				cout <<"Criar\n";
				tela_um.criar_conta(ssl, banco);
			}else{
				if (strcmp(buffer_server, "Remover ") == 0){
					cout <<"Remover\n";
					tela_um.remover(ssl, banco);					
				}else{
					if (strcmp(buffer_server, "Acessar ") == 0){
						cout <<"Acessar\n";
						tela_um.acessar(ssl, banco);					
					}else{
						if (strcmp(buffer_server, "Depositar ") == 0){
							cout <<"Depositar\n";
							tela_um.depositar(ssl, banco);					
						}else{
							if (strcmp(buffer_server, "Sair") == 0){
								cout <<"Sair\n";
								//Conexão do cliente Fechar 
								//close(ssl);				
							}
						}
					}
				}
			}
		}
        else ERR_print_errors_fp(stderr);   
    }
}

void socket_server::encerra_s(){

     //Feche o soquete local
    close(this->serverfd);

    cout << "Conexao encerrada"<< endl;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
menu_um::menu_um(){
	
};

void menu_um::criar_conta(SSL* ssl, MYSQL banco){
	SSL_write(ssl, "Voce selecionou 'Criar conta'.", BUFFER_LENGTH);
		
	for(int i = 0; i < 5; i++){
		if((contador = SSL_read(ssl, dados_cliente[i], BUFFER_LENGTH)) > 0){
			dados_cliente[i][contador - 1] = '\0';
			cout << "Cliente disse:"<< dados_cliente[i]<<endl;
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
	
	if (!res_banco) cout <<"Registros inseridos "<< mysql_affected_rows(&banco)<<endl;
    else cout <<"Erro na inserção "<< mysql_errno(&banco)<<" : " << mysql_error(&banco) << endl;
    
    bzero(str_banco,BUFFER_LENGTH);
        
	strcpy(str_banco, "SELECT cliente.ID_Cliente FROM cliente WHERE nome = '");
	strcat(str_banco,dados_cliente[0]);
	strcat(str_banco,"';");

	if (mysql_query(&banco,str_banco)){
		cerr << "Erro: " << mysql_error(&banco)<<endl;
	}else{
		res_consulta = mysql_store_result(&banco);//recebe a consulta
		if (res_consulta) //se houver consulta
        {
			dados_banco=mysql_fetch_row(res_consulta);
		}
	}
	
	bzero(str_banco,BUFFER_LENGTH);
	
    strcpy(str_banco,"INSERT INTO conta(senha, saldo, tipo, C_ID_Cliente) values('");
	strcat(str_banco, dados_cliente[3]);
	strcat(str_banco, "','100','");
	strcat(str_banco, dados_cliente[4]);
	strcat(str_banco, "','");
	strcat(str_banco, dados_banco[0]);
	strcat(str_banco, "');");
	
	res_banco = mysql_query(&banco,str_banco);
	
	if (!res_banco){
		cout << "Registros inseridos "<< mysql_affected_rows(&banco)<<endl;
		bzero(str_banco,BUFFER_LENGTH);
		strcpy(str_banco, "SELECT conta.ID_Conta FROM conta INNER JOIN cliente ON cliente.ID_Cliente = ");
		strcat(str_banco,dados_banco[0]);
		strcat(str_banco," AND conta.C_ID_Cliente = cliente.ID_Cliente;");

		bzero(dados_banco,BUFFER_LENGTH);
		if (mysql_query(&banco,str_banco)){
			cerr << "Erro: " << mysql_error(&banco)<<endl;
		}else{
			res_consulta = mysql_store_result(&banco);//recebe a consulta
			if (res_consulta) //se houver consulta
			{
				dados_banco=mysql_fetch_row(res_consulta);
				SSL_write(ssl, dados_banco[0], BUFFER_LENGTH);
			}
		}
    }else{
		cerr << "Erro na inserção "<< mysql_errno(&banco)<< " : "<< mysql_error(&banco) << endl;
    }
};

void menu_um::remover(SSL* ssl, MYSQL banco){
	SSL_write(ssl, "Voce selecionou 'Remover conta'.", BUFFER_LENGTH);
};

void menu_um::acessar(SSL* ssl, MYSQL banco){
	SSL_write(ssl, "Voce selecionou 'Acessar a conta'.", BUFFER_LENGTH);
	
	bzero(str_banco,BUFFER_LENGTH);
	bzero(buffer_respostas,BUFFER_LENGTH);
	menu_dois tela_dois;
	
	for (int i = 0; i < 2; i++){
		if((contador = SSL_read(ssl, dados_cliente[i], BUFFER_LENGTH)) > 0){
			dados_cliente[i][contador - 1] = '\0';
			cout << "Cliente disse: " << dados_cliente[i] << endl;
		}
	}
	
	//descobre o nome do cliente que esta fazendo login com base no id da conta e da senha
	strcpy(str_banco, "SELECT cliente.nome FROM conta INNER JOIN cliente ON conta.ID_conta = ");
	strcat(str_banco,dados_cliente[0]);
	strcat(str_banco," AND conta.senha = ");
	strcat(str_banco,dados_cliente[1]);
	strcat(str_banco," AND conta.C_ID_Cliente = cliente.ID_Cliente;");

	if (mysql_query(&banco,str_banco)){
		cerr << "Erro: " << mysql_error(&banco);
	}else{
		//se caso nao deu erro na consulta, independente de ter retornado positivo ou negativo, entra aqui
		res_consulta = mysql_store_result(&banco);//recebe a consulta
		if ((dados_banco=mysql_fetch_row(res_consulta)) != NULL){
			contador = 0;
			strcpy(dados_cliente[2], dados_banco[0]);            
            SSL_write(ssl, dados_banco[0], BUFFER_LENGTH);
            //criar outra clase para esses codigos IF pois sao o menu 2
            if((contador = SSL_read(ssl, buffer_respostas, BUFFER_LENGTH)) > 0){
				if(strcmp(buffer_respostas, "Saldo") == 0){
					cout << "Acesso -> Saldo \n";
					tela_dois.obtem_saldo(ssl, banco, dados_cliente);
				}else{
					if (strcmp(buffer_respostas, "Saque") == 0){
						cout << "Acesso -> Saque \n";
						tela_dois.sacar(ssl, banco, dados_cliente);					
					}else{
						if (strcmp(buffer_respostas, "Transferencia") == 0){
							cout << "Acesso -> Transferencia \n";
							tela_dois.transferir(ssl, banco, dados_cliente);					
						}else{
							if (strcmp(buffer_respostas, "Sair") == 0){
								cout << "Acesso -> Sair \n";					
							}
						}
					}
				}
				
			}
		}
    }
};

void menu_um::depositar(SSL* ssl, MYSQL banco){
	SSL_write(ssl, "Voce selecionou 'Depositar'.", BUFFER_LENGTH);
	
	for (int i = 0; i < 2; i++){
		if((contador = SSL_read(ssl, dados_cliente[i], BUFFER_LENGTH)) > 0){
		 dados_cliente[i][contador - 1] = '\0';
         cout << "Cliente disse:"<< dados_cliente[i] << endl;
		}
	}
	
	//monta a string de selecao ja somando o valor a ser depositado
	strcpy(str_banco, "SELECT cliente.nome FROM conta INNER JOIN cliente ON conta.ID_conta = ");
	strcat(str_banco,dados_cliente[0]);
	strcat(str_banco," AND conta.C_ID_Cliente = cliente.ID_Cliente;");
	
	//realiza a busca no banco de dados, trata erro ou confirma e armazena os resultados em *dados_banco
	// onde linha[0] e' o saldo na conta+ o valor a ser depositado e linha[1] e' o nome do beneficiado
	if (mysql_query(&banco,str_banco)){
		cerr << "Erro: "<< mysql_error(&banco);
	}else{
		res_consulta = mysql_store_result(&banco);//recebe a consulta
		if (res_consulta) //se houver consulta
        {
			dados_banco=mysql_fetch_row(res_consulta);
			SSL_write(ssl, dados_banco[0], BUFFER_LENGTH);
		}
	}
	
	//recebe confirmacao de insercao
	SSL_read(ssl, buffer_respostas, BUFFER_LENGTH);
	
	if (strcmp(buffer_respostas, "sim") == 0){
		bzero(str_banco,BUFFER_LENGTH);		
		strcpy(str_banco, "UPDATE `conta` SET `saldo` = saldo + ");
		strcat(str_banco, dados_cliente[1]);
		strcat(str_banco, " WHERE `ID_Conta` = ");
		strcat(str_banco, dados_cliente[0]);
		strcat(str_banco, ";");
		res_banco = mysql_query(&banco,str_banco);
		if (!res_banco) cout << "Registros alterados "<< mysql_affected_rows(&banco)<<endl;
		else cerr << "Erro na inserção "<< mysql_errno(&banco) << " : " << mysql_error(&banco);
	}
	
	//falta tratar erro quando o numero da conta não existe
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
menu_dois::menu_dois(){
	
};

void menu_dois::obtem_saldo(SSL* ssl, MYSQL banco, char dados_acesso[3][BUFFER_LENGTH]){
	SSL_write(ssl, "Você selecionou 'Saldo'.", BUFFER_LENGTH);
	
	//monta a string de selecao para descobrir o saldo e envia ao cliente
	bzero(str_banco,BUFFER_LENGTH);
	strcpy(str_banco, "SELECT conta.saldo FROM conta WHERE conta.ID_conta = ");
	strcat(str_banco, dados_acesso[0]);
	strcat(str_banco, ";");

	if (mysql_query(&banco,str_banco)){
		cerr << "Erro: "<< mysql_error(&banco);
	}else{
		res_consulta = mysql_store_result(&banco);//recebe a consulta
		if (res_consulta) //se houver consulta
        {
			dados_banco=mysql_fetch_row(res_consulta);
			cout << "Saldo: "<< dados_banco[0]<< endl;
			SSL_write(ssl, dados_banco[0], BUFFER_LENGTH);
		}
	}
	
};

void menu_dois::sacar(SSL* ssl, MYSQL banco, char dados_acesso[3][BUFFER_LENGTH]){
	SSL_write(ssl, "Você selecionou 'Sacar'.", BUFFER_LENGTH);
	
	//apresentar o saldo primeiro e depois perguntar qual valor deseja transferir
	//monta a string de selecao para descobrir o saldo e envia ao cliente
	bzero(str_banco,BUFFER_LENGTH);
	strcpy(str_banco, "SELECT conta.saldo FROM conta WHERE conta.ID_conta = ");
	strcat(str_banco, dados_acesso[0]);
	strcat(str_banco, ";");

	if (mysql_query(&banco,str_banco)){
		cerr << "Erro: "<< mysql_error(&banco);
	}else{
		res_consulta = mysql_store_result(&banco);//recebe a consulta
		if (res_consulta) //se houver consulta
        {
			dados_banco=mysql_fetch_row(res_consulta);
			SSL_write(ssl, dados_banco[0], BUFFER_LENGTH);
		}
	}
	if((contador = SSL_read(ssl, dados_cliente, BUFFER_LENGTH)) > 0){
        cout << "Cliente disse: " << dados_cliente[0]<<endl;
		
		bzero(str_banco,BUFFER_LENGTH);		
		strcpy(str_banco, "UPDATE `conta` SET `saldo` = saldo - ");
		strcat(str_banco, dados_cliente[0]);
		strcat(str_banco, " WHERE `ID_Conta` = ");
		strcat(str_banco, dados_acesso[0]);
		strcat(str_banco, ";");
		res_banco = mysql_query(&banco,str_banco);
		if (!res_banco){
			cout << "Registros alterados "<< mysql_affected_rows(&banco)<< endl;
			//apresentar o saldo primeiro e depois perguntar qual valor deseja transferir
			//monta a string de selecao para descobrir o saldo e envia ao cliente
			bzero(str_banco,BUFFER_LENGTH);
			strcpy(str_banco, "SELECT conta.saldo FROM conta WHERE conta.ID_conta = ");
			strcat(str_banco, dados_acesso[0]);
			strcat(str_banco, ";");

			if (mysql_query(&banco,str_banco)){
				cerr << "Erro: "<< mysql_error(&banco);
			}else{
				res_consulta = mysql_store_result(&banco);//recebe a consulta
				if (res_consulta) //se houver consulta
				{
					dados_banco=mysql_fetch_row(res_consulta);
					SSL_write(ssl, dados_banco[0], BUFFER_LENGTH);
				}
			}
		}else{
			cerr << "Erro na inserção "<< mysql_errno(&banco) <<" : "<< mysql_error(&banco);
		}
	}
	
};

void menu_dois::transferir(SSL* ssl, MYSQL banco, char dados_acesso[3][BUFFER_LENGTH]){
	SSL_write(ssl, "Você selecionou 'Transferir'.", BUFFER_LENGTH);
	
	//apresentar o saldo primeiro e depois perguntar qual valor deseja transferir
	//monta a string de selecao para descobrir o saldo e envia ao cliente
	bzero(str_banco,BUFFER_LENGTH);
	strcpy(str_banco, "SELECT conta.saldo FROM conta WHERE conta.ID_conta = ");
	strcat(str_banco, dados_acesso[0]);
	strcat(str_banco, ";");

	if (mysql_query(&banco,str_banco)){
		cerr << "Erro: "<< mysql_error(&banco);
	}else{
		res_consulta = mysql_store_result(&banco);//recebe a consulta
		if (res_consulta) //se houver consulta
        {
			dados_banco=mysql_fetch_row(res_consulta);
			SSL_write(ssl, dados_banco[0], BUFFER_LENGTH);
		}
	}
	
	//recebe o valor e a conta que sera beneficiada
	for (int i = 0; i < 2; i++){
		if((contador = SSL_read(ssl, dados_cliente, BUFFER_LENGTH)) > 0){
		 dados_cliente[i][contador - 1] = '\0';
         cout << "Cliente disse: "<< dados_cliente[i]<< endl;
		}
	}
	
	//monta a string de selecao para descobrir o nome do beneficiado
	strcpy(str_banco, "SELECT cliente.nome FROM conta INNER JOIN cliente ON conta.ID_conta = ");
	strcat(str_banco,dados_cliente[0]);
	strcat(str_banco," AND conta.C_ID_Cliente = cliente.ID_Cliente;");
	
	//realiza a busca no banco de dados, trata erro ou confirma e armazena os resultados em *dados_banco
	// onde linha[0] e' o (saldo na conta+ o valor a ser depositado) e linha[1] e' o nome do beneficiado
	if (mysql_query(&banco,str_banco)){
		cerr << "Erro: "<< mysql_error(&banco);
	}else{
		res_consulta = mysql_store_result(&banco);//recebe a consulta
		if (res_consulta) //se houver consulta
        {
			dados_banco=mysql_fetch_row(res_consulta);
			SSL_write(ssl, dados_banco[0], BUFFER_LENGTH);
		}
	}
	
	//recebe confirmacao de insercao
	SSL_read(ssl, buffer_respostas, BUFFER_LENGTH);

	if (strcmp(buffer_respostas, "sim") == 0){
		//insere valor na conta do beneficiado
		bzero(str_banco,BUFFER_LENGTH);		
		strcpy(str_banco, "UPDATE `conta` SET `saldo` = saldo + ");
		strcat(str_banco, dados_cliente[1]);
		strcat(str_banco, " WHERE `ID_Conta` = ");
		strcat(str_banco, dados_cliente[0]);
		strcat(str_banco, ";");
		res_banco = mysql_query(&banco,str_banco);
		if (!res_banco) cout << "Registros alterados "<< mysql_affected_rows(&banco)<<endl;
		else cerr << "Erro na inserção "<< mysql_errno(&banco) << " : " << mysql_error(&banco);
		
		//descontar o valor transferido da conta atual
		bzero(str_banco,BUFFER_LENGTH);		
		strcpy(str_banco, "UPDATE `conta` SET `saldo` = saldo - ");
		strcat(str_banco, dados_cliente[1]);
		strcat(str_banco, " WHERE `ID_Conta` = ");
		strcat(str_banco, dados_acesso[0]);
		strcat(str_banco, ";");
		res_banco = mysql_query(&banco,str_banco);
		if (!res_banco) cout << "Registros alterados "<< mysql_affected_rows(&banco)<<endl;
		else cerr << "Erro na inserção "<< mysql_errno(&banco) << " : " << mysql_error(&banco);
		
		//apresentar o saldo
		//monta a string de selecao para descobrir o saldo e envia ao cliente
		bzero(str_banco,BUFFER_LENGTH);
		strcpy(str_banco, "SELECT conta.saldo FROM conta WHERE conta.ID_conta = ");
		strcat(str_banco, dados_acesso[0]);
		strcat(str_banco, ";");

		if (mysql_query(&banco,str_banco)){
			cerr << "Erro: "<< mysql_error(&banco);
		}else{
			res_consulta = mysql_store_result(&banco);//recebe a consulta
			if (res_consulta) //se houver consulta
			{
				dados_banco=mysql_fetch_row(res_consulta);
				SSL_write(ssl, dados_banco[0], BUFFER_LENGTH);
			}
		}
	}
};

void menu_dois::extrato(SSL* ssl, MYSQL banco, char dados_acesso[3][BUFFER_LENGTH]){
	SSL_write(ssl, "Você selecionou 'Extrato'.", BUFFER_LENGTH);
};
