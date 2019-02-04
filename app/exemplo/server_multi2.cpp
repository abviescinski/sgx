//Example code: A simple server side code, which echos back the received message. 
//Handle multiple socket connections with select and fd_set on Linux  
#include <stdio.h>  //ok
#include <string.h>   //strlen   //ok
#include <stdlib.h>  //ok 
#include <errno.h>  //ok
#include <unistd.h>   //close  //ok
#include <arpa/inet.h>    //close  
#include <sys/types.h>  //ok
#include <sys/socket.h>  //ok
#include <netinet/in.h>  //ok
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros  
     
#define TRUE   1  
#define FALSE  0  
#define PORT 8080 

/* Tamanho de buffer do socket */
#define BUFFER_LENGTH 128 
     
int main(int argc , char *argv[])   
{   
    int opt = TRUE;   
    int master_socket , addrlen , new_socket , client_socket[30] ,  
          max_clients = 30 , activity, i , valread , sd;   
    int max_sd;   
    struct sockaddr_in server;   
         
    char buffer_serv[BUFFER_LENGTH];  //data buffer of 1K  
         
    //conjunto de descritores de soquete  
    fd_set readfds;   
         
    //uma mensagem 
    char message[BUFFER_LENGTH];  
    strcpy(message, "ECHO Daemon v1.0 \r\n");
    
    //inicializar todos os clientsocket[] para 0, por isso não está marcado
    for (i = 0; i < max_clients; i++)   
    {   
        client_socket[i] = 0;   
    }   
         
    //criar um soquete principal 
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)   
    {   
        perror("socket failed");   
        exit(EXIT_FAILURE);   
    }   
     
    //set socket master para permitir múltiplas conexões, isso é apenas um bom hábito, ele vai funcionar sem isso 
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )   
    {   
        perror("setsockopt");   
        exit(EXIT_FAILURE);   
    }   
     
    //tipo de soquete criado
    server.sin_family = AF_INET;   
    server.sin_addr.s_addr = INADDR_ANY;   
    server.sin_port = htons( PORT );   
         
    //bind the socket to localhost port 8888  
    if (bind(master_socket, (struct sockaddr *)&server, sizeof(server))<0)   
    {   
        perror("bind failed");   
        exit(EXIT_FAILURE);   
    }   
    printf("Listener on port %d \n", PORT);   
         
    //tente especificar o máximo de 3 conexões pendentes para o soquete principal
    if (listen(master_socket, 3) < 0)   
    {   
        perror("listen");   
        exit(EXIT_FAILURE);   
    }   
         
    //aceitar a conexão de entrada  
    addrlen = sizeof(server);   
    puts("Waiting for connections ...");   
         
    while(TRUE)   
    {   
        //limpar o conjunto de soquetes  
        FD_ZERO(&readfds);   
     
        //adicionar soquete principal para definir
        FD_SET(master_socket, &readfds);   
        max_sd = master_socket;   
             
        //adicionar soquetes filho para definir
        for ( i = 0 ; i < max_clients ; i++)   
        {   
            //descritor de socket
            sd = client_socket[i];   
                 
            //if valid socket descriptor then add to read list  
            if(sd > 0)   
                FD_SET( sd , &readfds);   
                 
            //maior número de descritor de arquivo, precisa dele para a função select
            if(sd > max_sd)   
                max_sd = sd;   
        }   
     
        //esperar por uma atividade em um dos soquetes, o tempo limite é NULL, então espere indefinidamente
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);   
       
        if ((activity < 0) && (errno!=EINTR))   
        {   
            printf("select error");   
        }   
             
        //Se algo aconteceu no soquete principal, então é uma conexão de entrada  
        if (FD_ISSET(master_socket, &readfds))   
        {   
            if ((new_socket = accept(master_socket, (struct sockaddr *)&server, (socklen_t*)&addrlen))<0)   
            {   
                perror("accept");   
                exit(EXIT_FAILURE);   
            }   
             
            //informar usuário do número do socket - usado nos comandos send e receive 
            printf("New connection , socket fd is %d , ip is : %s , port : %d \n" , new_socket , inet_ntoa(server.sin_addr) , ntohs(server.sin_port));   
           
            //enviar nova mensagem de saudação de conexão
            send(new_socket, message, BUFFER_LENGTH, 0) ;
            /*if( send(new_socket, message, strlen(message), 0) != strlen(message) )   
            {   
                perror("send");   
            } */  
                 
            puts("Welcome message sent successfully");   
                 
            //adicionar novo soquete à matriz de soquetes
            for (i = 0; i < max_clients; i++)   
            {   
                //se a posição estiver vazia
                if( client_socket[i] == 0 )   
                {   
                    client_socket[i] = new_socket;   
                    printf("Adding to list of sockets as %d\n" , i);   
                         
                    break;   
                }   
            }   
        }   
             
        //outra parte sua operação IO em algum outro soquete
        for (i = 0; i < max_clients; i++)   
        {   
            sd = client_socket[i];   
                 
            if (FD_ISSET( sd , &readfds))   
            {   
                //Verifique se foi para fechar e também leia a mensagem recebida 
                if ((valread = read( sd , buffer_serv, 1024)) == 0)   
                {   
                    //Alguém desconectou, conseguiu seus detalhes e imprimiu 
                    getpeername(sd , (struct sockaddr*)&server , (socklen_t*)&addrlen);   
                    printf("Host disconnected , ip %s , port %d \n" ,  
                          inet_ntoa(server.sin_addr) , ntohs(server.sin_port));   
                         
                    //Feche o socket e marque como 0 na lista para reutilizar
                    //close( sd );   
                    client_socket[i] = 0;   
                }   
                     
                //Eco de volta a mensagem que veio em
                else 
                {   
                    //definir a cadeia de terminação de bytes NULL no final dos dados lidos
                    buffer_serv[valread] = '\0';   
                    send(sd , buffer_serv , BUFFER_LENGTH , 0 );   
                }   
            }   
        }   
    }   
         
    return 0;   
}
