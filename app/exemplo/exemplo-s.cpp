#include "ServerSocket.h"
  #include "SocketException.h"
  #include
  
  int main ( int argc, int argv[] )
  {
	  try
	  {
	  // Criação do server socket na porta 30000
	  ServerSocket server ( 30000 );
	  
	  while ( true )
	  {
	  // Aceitando a conexão solicitada
	  ServerSocket new_sock;
	  server.accept ( new_sock );
	  
	  try
	  {
	  while ( true )
	  {
		  std::string data;
		  // Recebendo dados
		  new_sock >> data;
		  // Enviando dados
		  new_sock << data;
	  }
	  catch ( SocketException& ) {}
	  }
	  catch ( SocketException& e )
	  {
	  // Tratamento de erro
	  std::cout << "Excecão foi capturada:" << e.description() << " Finalizando. ";
	  }
	  
	  return 0;
  }
