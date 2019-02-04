#include "ClientSocket.h"
  #include "SocketException.h"
  #include
  #include
  
  int main ( int argc, int argv[] )
  {
  try
  {
  // Criando um client socket e solicitando a conexão na porta 30000 e IP local.
  ClientSocket client_socket ( "localhost", 30000 );
  
  std::string reply;
  try
  {
  // Envia a string "Test message" ao server
  client_socket << "Test message.";
  
  // Recebe dados do server
  client_socket >> reply;
  std::cout< catch ( SocketException& ) {}
  
  }
  catch ( SocketException& e )
  {
  std::cout << "Excecão foi capturada:" << e.description() << " ";
  }
  
  return 0;
  }
