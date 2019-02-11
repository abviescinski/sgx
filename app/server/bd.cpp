
//cria conta 
{
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
}


//ACESSA CONTA
{
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
}

//DEPOSITA
{
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
}

//OBTEM SALDO
{
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
}

//SACAR
{
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
}

//TRANSFERIR
{
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
}


//
