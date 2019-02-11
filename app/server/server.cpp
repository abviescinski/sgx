#include "server.hpp"

socket_server::socket_server(){
	
	
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SSL_CTX* socket_server::InitServerCTX(void){
	const SSL_METHOD *method;
    SSL_CTX *ctx;
    
	cout << "Inicio server CTX, carregar as coisas do OpenSSL\n";
	
    OpenSSL_add_all_algorithms();  /* load & register all cryptos, etc. */
    SSL_load_error_strings();   /* load all error messages */
    //method = TLS_server_method();  /* create new server-method instance */
    method = TLSv1_server_method();  /* create new server-method instance */
    ctx = SSL_CTX_new(method);   /* create new context from method */
    
    cout<<"Finalizou os carregamentos SSL, verifica se deu tudo certo.\n";
    
    if ( ctx == NULL ){
        ERR_print_errors_fp(stderr);
        abort();
    }
    
    cout << "Finaliza a função Init server e retorna ctx.\n";
    return ctx;
}

void socket_server::LoadCertificates(SSL_CTX* ctx, const char* CertFile, const char* KeyFile){
	// set the local certificate from CertFile
    if ( SSL_CTX_use_certificate_file(ctx, CertFile, SSL_FILETYPE_PEM)<=0){
        ERR_print_errors_fp(stderr);
        abort();
    }
    
    cout << "Load Certificates, verifica o pem do certificado se esta correto\n";
    
    //set the private key from KeyFile (may be the same as CertFile)
    if ( SSL_CTX_use_PrivateKey_file(ctx, KeyFile, SSL_FILETYPE_PEM)<=0){
        ERR_print_errors_fp(stderr);
        abort();
    }
    
    cout << "Load Certificates, verifica o se pem do chave privada esta correto\n";
    
    /* verify private key */
    if (!SSL_CTX_check_private_key(ctx)){
        cout << "Private key does not match the public certificate\n";
        abort();
    }
    
    cout <<"Finaliza a função LoadCertificates, tudo certo ate aqui\n";
}

void socket_server::ShowCerts(SSL* ssl){
	X509 *cert;
    char *line;
 
	cout <<"Apresenta certificados \n";
 
    cert = SSL_get_peer_certificate(ssl); /* Get certificates (if available) */
    
    cout <<"Obter certificados (se disponíveis)\n";
    
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
    else cout << "LADO SERVIDOR, sem certificados!\n";
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
	char info_cliente[6][BUFFER_LENGTH];
	char x[2][BUFFER_LENGTH];
	
	for(int i = 0; i < 5; i++){
		if((contador = SSL_read(ssl, info_cliente[i], BUFFER_LENGTH)) > 0){
			info_cliente[i][contador - 1] = '\0';
			cout << "Cliente disse:"<< info_cliente[i]<<endl;
		}
	}

	if((str_bd.insert_bd(banco, NovoCliente, info_cliente)) == 0){
		dados_banco = str_bd.select_bd(banco, IDcliente, info_cliente[1]);
		strcpy(info_cliente[5], dados_banco[0]);

		if ((str_bd.insert_bd(banco, NovaConta, info_cliente)) == 0){
			strcpy(x[0], info_cliente[5]);
			dados_banco = str_bd.innerjoin_bd(banco, IDconta, x);
			SSL_write(ssl, dados_banco[0], BUFFER_LENGTH);
		}else{
			cerr << "Erro na inserção da conta - "<< mysql_errno(&banco)<< " : "<< mysql_error(&banco) << endl;
		}
	}else{
		cerr << "Erro na inserção do cliente - "<< mysql_errno(&banco)<< " : "<< mysql_error(&banco) << endl;
	}
}

void menu_um::remover(SSL* ssl, MYSQL banco){
	SSL_write(ssl, "Voce selecionou 'Remover conta'.", BUFFER_LENGTH);
};

void menu_um::acessar(SSL* ssl, MYSQL banco){
	SSL_write(ssl, "Voce selecionou 'Acessar a conta'.", BUFFER_LENGTH);
	char info_cliente[2][BUFFER_LENGTH];
	
	bzero(buffer_respostas,BUFFER_LENGTH);
	menu_dois tela_dois;
	
	for (int i = 0; i < 2; i++){
		if((contador = SSL_read(ssl, info_cliente[i], BUFFER_LENGTH)) > 0){
			info_cliente[i][contador - 1] = '\0';
			cout << "Cliente disse: " << info_cliente[i] << endl;
		}
	}
	
	//retorna o nome do cliente se dados de login estiver corretos
	dados_banco = str_bd.innerjoin_bd(banco, Login, info_cliente);
	
	contador = 0;
	strcpy(info_cliente[2], dados_banco[0]);            
    SSL_write(ssl, dados_banco[0], BUFFER_LENGTH);
    
    //criar outra clase para esses codigos IF pois sao o menu 2
    if((contador = SSL_read(ssl, buffer_respostas, BUFFER_LENGTH)) > 0){
		if(strcmp(buffer_respostas, "Saldo") == 0){
			cout << "Acesso -> Saldo \n";
			tela_dois.obtem_saldo(ssl, banco, info_cliente[0]);
		}else{
			if (strcmp(buffer_respostas, "Saque") == 0){
				cout << "Acesso -> Saque \n";
				tela_dois.sacar(ssl, banco, info_cliente);					
			}else{
				if (strcmp(buffer_respostas, "Transferencia") == 0){
					cout << "Acesso -> Transferencia \n";
					tela_dois.transferir(ssl, banco, info_cliente);					
				}else{
					if (strcmp(buffer_respostas, "Sair") == 0){
						cout << "Acesso -> Sair \n";					
					}
				}
			}
		}		
	}
};

void menu_um::depositar(SSL* ssl, MYSQL banco){
	SSL_write(ssl, "Voce selecionou 'Depositar'.", BUFFER_LENGTH);
	bzero(buffer_respostas,BUFFER_LENGTH);
	char info_cliente[2][BUFFER_LENGTH];
	
	for (int i = 0; i < 2; i++){
		if((contador = SSL_read(ssl, info_cliente[i], BUFFER_LENGTH)) > 0){
		 info_cliente[i][contador - 1] = '\0';
         cout << "Cliente disse:"<< info_cliente[i] << endl;
		}
	}
	//recebe o nome completo do beneficiado
	dados_banco = str_bd.innerjoin_bd(banco, Nome,info_cliente);
	SSL_write(ssl, dados_banco[0], BUFFER_LENGTH);

	//recebe confirmacao de insercao
	SSL_read(ssl, buffer_respostas, BUFFER_LENGTH);
	
	if (strcmp(buffer_respostas, "sim") == 0){
		str_bd.update_bd(banco, Soma, info_cliente);
	}
	
	//falta tratar erro quando o numero da conta não existe
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
menu_dois::menu_dois(){
	
};

void menu_dois::obtem_saldo(SSL* ssl, MYSQL banco, char dados_acesso[BUFFER_LENGTH]){
	SSL_write(ssl, "Você selecionou 'Saldo'.", BUFFER_LENGTH);
	
	dados_banco = str_bd.select_bd(banco, Saldo, dados_acesso);
	
	cout << "Saldo: "<< dados_banco[0]<< endl;
	SSL_write(ssl, dados_banco[0], BUFFER_LENGTH);	
};

void menu_dois::sacar(SSL* ssl, MYSQL banco, char dados_acesso[3][BUFFER_LENGTH]){
	SSL_write(ssl, "Você selecionou 'Sacar'.", BUFFER_LENGTH);
	char info_cliente[2][BUFFER_LENGTH];
	strcpy(info_cliente[1], dados_acesso[0]);
	
	//retornar saldo atual para realizar ação
	dados_banco = str_bd.select_bd(banco, Saldo, dados_acesso[0]);
	cout << "Saldo: "<< dados_banco[0]<< endl;
	SSL_write(ssl, dados_banco[0], BUFFER_LENGTH);

	//recebe o valor que deseja sacar 
	if((contador = SSL_read(ssl, info_cliente[0], BUFFER_LENGTH)) > 0){
        cout << "Cliente disse: " << info_cliente[0] <<endl;
		
		res_banco = str_bd.update_bd(banco, Subtrai, info_cliente);
		
		bzero(dados_banco,sizeof(dados_banco));
		dados_banco = str_bd.select_bd(banco, Saldo, dados_acesso[0]);
	
		cout << "Saldo: "<< dados_banco[0]<< endl;
		SSL_write(ssl, dados_banco[0], BUFFER_LENGTH);
	}
};

void menu_dois::transferir(SSL* ssl, MYSQL banco, char dados_acesso[3][BUFFER_LENGTH]){
	SSL_write(ssl, "Você selecionou 'Transferir'.", BUFFER_LENGTH);
	char info_cliente[2][BUFFER_LENGTH];
	
	//retornar saldo atual para realizar ação
	dados_banco = str_bd.select_bd(banco, Saldo, dados_acesso[0]);
	cout << "Saldo: "<< dados_banco[0]<< endl;
	SSL_write(ssl, dados_banco[0], BUFFER_LENGTH);	
	
	//recebe o valor e a conta que sera beneficiada
	for (int i = 0; i < 2; i++){
		contador=0;
		if((contador = SSL_read(ssl, info_cliente[i], BUFFER_LENGTH)) > 0){
		 info_cliente[i][contador - 1] = '\0';
         cout << "Cliente disse: "<< info_cliente[i]<< endl;
		}
	}
	
	//busca o nome do beneficiado
	bzero(dados_banco,sizeof(dados_banco));
	dados_banco = str_bd.innerjoin_bd(banco, Nome, info_cliente);
	SSL_write(ssl, dados_banco[0], BUFFER_LENGTH);
	
	//recebe confirmacao de insercao
	SSL_read(ssl, buffer_respostas, BUFFER_LENGTH);
	
	if (strcmp(buffer_respostas, "sim") == 0){
		strcpy(dados_acesso[1], info_cliente[1]);
		
		int c = str_bd.update_bd(banco, Soma, info_cliente);
		int d = str_bd.update_bd(banco, Subtrai, dados_acesso);
		
		if ((c==0)&&(d==0)){		
			bzero(dados_banco,sizeof(dados_banco));
			dados_banco=str_bd.select_bd(banco, Saldo, dados_acesso[0]);
			SSL_write(ssl, dados_banco[0], BUFFER_LENGTH);
		}else{
			cerr<<"Erro ao realizar atualizações de saldo\n";
		}
	}
};

void menu_dois::extrato(SSL* ssl, MYSQL banco, char dados_acesso[3][BUFFER_LENGTH]){
	SSL_write(ssl, "Você selecionou 'Extrato'.", BUFFER_LENGTH);
};
