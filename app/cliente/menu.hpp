#include "client.hpp"

class menu_um{
	
	public:
		menu_um();
		void criar_conta(SSL* ssl);
		void acessar(SSL* ssl);
		void depositar(SSL* ssl);
		void remover(SSL* ssl);
		
	//opcao: opcao pro cliente selecionar (funcoes acima)
	//buffer_client recebe a confirmacao do servidor na opcao selecionada
	char opcao[20], buffer_cliente[LEN];
	int resposta_servidor;
	char cliente[5][LEN];
	char conta[2][LEN];

};

class menu_dois{
	
	public:
		menu_dois();
		void opcoes(SSL* ssl, char nome_cliente[LEN]);
		void obter_saldo(SSL* ssl, char nome_cliente[LEN]);
		void sacar(SSL* ssl, char nome_cliente[LEN]);
		void transferir(SSL* ssl, char nome_cliente[LEN]);
		void extrato(SSL* ssl, char nome_cliente[LEN]);
	
	char opcao[20], buffer_cliente[LEN],conta[2][LEN];
	int resposta_servidor, tipo;
};


//logoff(conta);

//log();
