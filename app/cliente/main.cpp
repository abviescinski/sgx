#include "menu.hpp"

int main(int argc, char *argv[]) 
{
	socket_cliente socket;
	menu_um   tela_um;
	
	int opc, canal;
	
	canal = socket.inicia_conexao();
	
	cout <<"Selecione uma opção:" << endl;
	cout <<"	1 - Criar conta." << endl;
	cout <<"	2 - Remover conta." << endl;
	cout <<"	3 - Acesso a conta." << endl;
	cout <<"	4 - Depositar." << endl;
	cout <<"	0 - Encerrar conexao." << endl;
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
			cout<< "Ate mais!"<< endl;
			socket.fecha_conexao();
			break;
		default:
			cout <<"Selecione uma opcao: "<< endl;
			cin >> opc;
			cin.ignore();
	}
	//colocar em loop para a pessoa selecionar as opcoes informadas ou encerrar o programa
		
}
