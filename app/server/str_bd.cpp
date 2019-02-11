#include "server.hpp"

str_banco::str_banco(){
	
}

MYSQL str_banco::conecta_bd(){
	MYSQL conexao;
    mysql_init(&conexao);
    
    if (mysql_real_connect(&conexao,HOST,USER,PASS,DB,0,NULL,0)){
         cout << "Conectado na base de dados com sucesso!\n";
         return conexao;
    }else{
		cout << "Falha de conexao\n";
        cerr << "Erro " << mysql_errno(&conexao) << " : " << mysql_error(&conexao);
    }
}

bool str_banco::insert_bd(MYSQL banco, int opc, char info[6][BUFFER_LENGTH]){
	bzero(solicita,BUFFER_LENGTH);
	switch (opc){
		case NovoCliente:
			strcpy(solicita, "INSERT INTO cliente(nome, cpf, telefone) values ('");
			strcat(solicita, info[0]);
			strcat(solicita, "','");
			strcat(solicita, info[1]);
			strcat(solicita, "','");
			strcat(solicita, info[2]);
			strcat(solicita, "');");
			break;
		case NovaConta:
			strcpy(solicita, "INSERT INTO conta(senha, saldo, tipo, C_ID_Cliente) values ('");
			strcat(solicita, info[3]);
			strcat(solicita, "','100','");
			strcat(solicita, info[4]);
			strcat(solicita, "','");
			strcat(solicita, info[5]);
			strcat(solicita, "');");
			break;
		default:
			cout << "Erro ao selecionar opcao 'INSERT'.\n";
			break;
	}
	
	res_banco = mysql_query(&banco,solicita);
	
	if (!res_banco){
		cout <<"Registros inseridos "<< mysql_affected_rows(&banco)<<endl;
		return EXIT_SUCCESS;
    }else{
		cout <<"Erro na inserção "<< mysql_errno(&banco)<<" : " << mysql_error(&banco) << endl;
		return EXIT_FAILURE;
	}
}

MYSQL_ROW str_banco::select_bd(MYSQL banco, int opc, char info[BUFFER_LENGTH]){

	bzero(solicita,BUFFER_LENGTH);
	switch (opc){
		case Saldo: //retorna valor do saldo disponivel
			strcpy(solicita, "SELECT conta.saldo FROM conta WHERE conta.ID_conta = ");
			strcat(solicita, info);
			strcat(solicita,";");
			break;
		case IDcliente: //retorna id do cliente conforme o cpf
			strcpy(solicita, "SELECT cliente.ID_Cliente FROM cliente WHERE cpf = ");
			strcat(solicita, info);
			strcat(solicita,";");
			break;
		default:
			cout << "Erro ao selecionar opcao 'SELECT'.\n";
			break;
	}
	
	if (mysql_query(&banco,solicita)){
		//cerr << "Erro: "<< mysql_error(&banco);
		exit (0);
	}else{
		res_consulta = mysql_store_result(&banco);//recebe a consulta
		if (res_consulta){ //se houver consulta
			dados_banco=mysql_fetch_row(res_consulta);
			return dados_banco;
		}
	}
}

MYSQL_ROW str_banco::innerjoin_bd(MYSQL banco, int opc, char info[2][BUFFER_LENGTH]){
	bzero(solicita,BUFFER_LENGTH);
	char confirma_cli[50] =  " AND conta.C_ID_Cliente = cliente.ID_Cliente;";
		
	switch (opc){
		case IDconta:
			strcpy(solicita, "SELECT conta.ID_Conta FROM conta INNER JOIN cliente ON cliente.ID_Cliente = ");
			strcat(solicita, info[0]);
			strcat(solicita, confirma_cli);
			break;
		case Nome:
			strcpy(solicita, "SELECT cliente.nome FROM conta INNER JOIN cliente ON conta.ID_conta = ");
			strcat(solicita, info[0]);
			strcat(solicita, confirma_cli);
			break;
		case Login:
			strcpy(solicita, "SELECT cliente.nome FROM conta INNER JOIN cliente ON conta.ID_conta = ");
			strcat(solicita, info[0]);
			strcat(solicita," AND conta.senha = ");
			strcat(solicita, info[1]);
			strcat(solicita, confirma_cli);
			break;
		default:
			cout << "Erro ao selecionar opcao 'INNER-JOIN'.\n";
			break;
	}
	
	if (mysql_query(&banco,solicita)){
		//cerr << "Erro: "<< mysql_error(&banco);
		//return mysql_error(&banco);
		exit(0);
	}else{
		res_consulta = mysql_store_result(&banco);//recebe a consulta
		if (res_consulta){ //se houver consulta
			dados_banco=mysql_fetch_row(res_consulta);
			return dados_banco;
		}
	}
}

bool str_banco::update_bd(MYSQL banco, int opc, char info[2][BUFFER_LENGTH]){
	bzero(solicita,BUFFER_LENGTH);
	
	switch (opc){
		case Soma:
			strcpy(solicita, "UPDATE `conta` SET `saldo` = saldo + ");
			strcat(solicita, info[1]);
			strcat(solicita, " WHERE `ID_Conta` = ");
			strcat(solicita, info[0]);
			strcat(solicita, ";");
			break;
		case Subtrai:
			strcpy(solicita, "UPDATE `conta` SET `saldo` = saldo - ");
			strcat(solicita, info[1]);
			strcat(solicita, " WHERE `ID_Conta` = ");
			strcat(solicita, info[0]);
			strcat(solicita, ";");
			break;
		default:
			cout << "Erro ao selecionar opcao 'UPDATE'.\n";
	}
	
	res_banco = mysql_query(&banco,solicita);
	
	if (!res_banco){
		cout <<"Registros atualizados "<< mysql_affected_rows(&banco)<<endl;
		return EXIT_SUCCESS;
    }else{
		cout <<"Erro na atualização "<< mysql_errno(&banco)<<" : " << mysql_error(&banco) << endl;
		return EXIT_FAILURE;
	}
}

bool str_banco::delet_bd(MYSQL banco, int opc){

}
