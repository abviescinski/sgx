#include "server.hpp"

int main (int argc, char *argv[])
{
   	socket_server soquete;
	MYSQL banco = soquete.conecta_bd();
	int cli = soquete.cria_s();
	soquete.conecta_s(banco, cli);
	soquete.encerra_s();
}
