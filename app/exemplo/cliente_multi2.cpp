// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>
#define PORT 8888
   
int main(int argc, char const *argv[]) 
{ 
   // struct sockaddr_in address; 
    int sockfd = 0; 
    struct sockaddr_in server; 
    char hello[1024] ; 
    strcpy(hello, "Hello from client");
    char buffer[1024] = {0}; 
    
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
   
    memset(&server, '0', sizeof(server)); 
   
    server.sin_family = AF_INET; 
    server.sin_port = htons(PORT); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, "127.0.0.1", &server.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
   
    if (connect(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 
    send(sockfd , hello , strlen(hello) , 0 ); 
    printf("Hello message sent\n"); 
    recv( sockfd , buffer, 1024,0); 
    printf("%s\n",buffer ); 
    return 0; 
} 
