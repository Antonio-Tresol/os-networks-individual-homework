

/**
  *  Server-side implementation of UDP client-server model	
  *
  *  CI-0123 Proyecto integrador de redes y sistemas operativos
  *
 **/
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "Socket.hpp" 

#define PORT	 9876 
#define MAXLINE 1024 

int main( int argc, char * argv[] ) {
   if (argc != 2) {
      printf("Usage: %s for server pass as argument 1, 0 for client", argv[0]);
      return -1;
   } else if (strcmp(argv[1], "1") == 0) {
      printf("-Server mode\n");
      Socket * server;
      int len, n; 
      int sockfd;
      struct sockaddr other;
      char buffer[MAXLINE]; 
      char *hello = (char *) "Hello from CI0123 server"; 
      
      server = new Socket( 'd', false );
      server->Bind( PORT );

      memset( &other, 0, sizeof( other ) );

      n = server->recvFrom( (void *) buffer, MAXLINE, (void *) &other );	// Mensaje de los www servers
      buffer[n] = '\0'; 
      printf("Server: message received: %s\n", buffer);

      server->sendTo( (const void *) hello, strlen( hello ), (void *) &other );
      printf("Server: Hello message sent.\n"); 

      server->Close();
      
      return 0;
   } else if (strcmp(argv[1], "0") == 0) {
      printf("Client mode\n");
      Socket * client;
      int sockfd; 
      int n, len; 
      char buffer[MAXLINE]; 
      char *hello = (char *) "Hello from client"; 
      struct sockaddr_in other;

      client = new Socket( 'd' );	// Creates an UDP socket: datagram

      memset( &other, 0, sizeof( other ) ); 
      
      other.sin_family = AF_INET; 
      other.sin_port = htons(PORT); 
      other.sin_addr.s_addr = INADDR_ANY; 
      
      n = client->sendTo( (void *) hello, strlen( hello ), (void *) & other ); 
      printf("Client: Hello message sent.\n"); 
      
      n = client->recvFrom( (void *) buffer, MAXLINE, (void *) & other );
      buffer[n] = '\0'; 
      printf("Client message received: %s\n", buffer); 

      client->Close(); 

      return 0;
   } else {
      printf("Usage: %s for server pass as argument 1, 0 for client", argv[0]);
      return -1;
   }
  
} 



