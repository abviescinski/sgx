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
	int resposta_servidor, tipo;
	char cliente[5][LEN];
	char conta[2][LEN];

};

class menu_dois{
	
	public:
		menu_dois();
		void obter_saldo(SSL* ssl);
		void sacar(SSL* ssl);
		void transferir(SSL* ssl);
		void extrato(SSL* ssl);
	
	char opcao[20], buffer_cliente[LEN],conta[2][LEN];
	int resposta_servidor;
};


//logoff(conta);

//log();
