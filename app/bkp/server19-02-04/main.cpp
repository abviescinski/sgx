#include "server.hpp"

int main(void) 
{
	socket_server soquete;
	
	MYSQL banco = soquete.conecta_bd();
	
	soquete.cria_s();
	
	soquete.conecta_s(banco);
	
	
	soquete.encerra_s();  
	
	
}
