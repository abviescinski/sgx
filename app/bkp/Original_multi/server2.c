/* A simple server in the internet domain using TCP
   The port number is passed as an argument 
   This version runs forever, forking off a separate 
   process for each connection
*/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define BUFFER_LENGTH 128

void dostuff(int); /* function prototype */
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
     int serverfd, clientfd, portno, pid;
     socklen_t client_len;
     struct sockaddr_in server, client;

     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     
     serverfd = socket(AF_INET, SOCK_STREAM, 0);//ok
     if (serverfd < 0) 
        error("ERROR opening socket");//ok
        
     //diferente   
     bzero((char *) &server, sizeof(server));
     portno = atoi(argv[1]); // não precisa pois é a conversao da porta passada como parametro
     //
     server.sin_family = AF_INET;//ok
     
     //diferente 
     server.sin_addr.s_addr = INADDR_ANY; //ok
     server.sin_port = htons(portno); //ok
     //
     
     if (bind(serverfd, (struct sockaddr *) &server, sizeof(server)) < 0) 
              error("ERROR on binding");//+/- ok
     
     //MUDA A PARTIR DAQUI
         
     listen(serverfd,5);
     client_len = sizeof(client);
     while (1) {
         clientfd = accept(serverfd, 
               (struct sockaddr *) &client, &client_len);
         if (clientfd < 0) 
             error("ERROR on accept");
         pid = fork();
         if (pid < 0)
             error("ERROR on fork");
         if (pid == 0)  {
             close(serverfd);
             dostuff(clientfd);
             exit(0);
         }
         else close(clientfd);
     } /* end of while */
     close(serverfd);
     return 0; /* we never get here */
}

/******** DOSTUFF() *********************
 There is a separate instance of this function 
 for each connection.  It handles all communication
 once a connnection has been established.
 *****************************************/
void dostuff (int sock)
{
   int n;
   char buffer_serv[BUFFER_LENGTH];
      
   bzero(buffer_serv,BUFFER_LENGTH);
   n = read(sock,buffer_serv,BUFFER_LENGTH);
   if (n < 0) error("ERROR reading from socket");
   printf("Here is the message: %s\n",buffer_serv);
   n = write(sock,"I got your message",BUFFER_LENGTH);
   if (n < 0) error("ERROR writing to socket");
}
