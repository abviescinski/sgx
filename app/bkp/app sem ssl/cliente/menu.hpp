#include "client.hpp"

class menu_um{
	
	public:
		menu_um();
		void criar_conta(int canal);
		void acessar(int canal);
		void depositar(int canal);
		void remover(int canal);
	
	//opcao: opcao pro cliente selecionar (funcoes acima)
	//buffer_client recebe a confirmacao do servidor na opcao selecionada
	char opcao[20], buffer_client[LEN];
	int resposta_servidor, tipo;
	char cliente[5][LEN];
	char conta[2][LEN];

};

class menu_dois{
	
	public:
		menu_dois();
		void obter_saldo(int canal);
		void sacar(int canal);
		void transferir(int canal);
		void extrato(int canal);
	
	char opcao[20], buffer_client[LEN],conta[2][LEN];
	int resposta_servidor;
};


//logoff(conta);

//log();
