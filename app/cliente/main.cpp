#include "menu.hpp"

int main(int argc, char **argv) 
{
	socket_cliente socket;
	menu_um   tela_um;
	int opc, canal;
	
	canal = socket.inicia_conexao(argc, argv);
	
	if (canal == 1){
		cout << "A comunicação com o servidor falhou! :( \n";
	}
	else
	{	
		cout <<"\nSelecione uma opção:\n";
		cout <<"	1 - Criar conta.\n";
		cout <<"	2 - Remover conta.\n";
		cout <<"	3 - Acesso a conta.\n";
		cout <<"	4 - Depositar.\n";
		cout <<"	0 - Encerrar conexao.\n";
		cin >> opc;
		
		cin.ignore();
		
		switch (opc) {
			case 1:
				tela_um.criar_conta(canal);
				break;
			case 2:
				tela_um.remover(canal);
				break;
			case 3:
				tela_um.acessar(canal);
				break;
			case 4:
				tela_um.depositar(canal);
				break;
			case 0:
				cout<< "Ate mais! \n";
				socket.fecha_conexao();
				break;
			default:
				cout <<"Selecione uma opcao: \n";
				cin >> opc;
				cin.ignore();
		}
	}
	//colocar em loop para a pessoa selecionar as opcoes informadas ou encerrar o programa
		
}
