#include "server.hpp"

int main ()
{
   	socket_server soquete;
	MYSQL banco = soquete.conecta_bd();
	soquete.cria_s(banco);
	soquete.encerra_s();
}
