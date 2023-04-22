/**
 *   UCR-ECCI
 *   CI-0123 Proyecto integrador de redes y sistemas operativos
 *
 *   Socket client/server example with threads
 *
 **/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>	// memset
#include <unistd.h>
#include <iostream>
#include <thread>

#include "Socket.hpp"

#define PORT 5678
#define BUFSIZE 512


/**
 *   Task each new thread will run
 *      Read string from socket
 *      Write it back to client
 *
 **/
void task( Socket * client ) {
   char a[ BUFSIZE ];

   client->Read( a, BUFSIZE );	// Read a string from client, data will be limited by BUFSIZE bytes
   std::cout << "Server received: " << a << std::endl;
   client->Write( a );		// Write it back to client, this is the mirror function
   client->Close();		// Close socket in parent

}


/**
 *   Create server code
 *      Infinite for
 *         Wait for client conection
 *         Spawn a new thread to handle client request
 *
 **/
int main( int argc, char ** argv ) {
  int mode = 0;
  // Get int from standard input
  std::cout << "Enter 0 to run as client, 1 to run as server Fork, 2 to run as server Thread: ";
  try {
    std::cin >> mode;
  } catch (std::exception& e) {
    std::cout << "Error: " << e.what() << std::endl;
    return 1;
  }

  if (mode == 2) {
    std::cout << "Running as server Thread" << std::endl;
    std::thread * worker;
    Socket s1( 's' ), * client;

    s1.Bind( PORT );		// Port to access this mirror server
    s1.Listen( 5 );		// Set backlog queue to 5 conections

    for( ; ; ) {
      client = s1.Accept();	 	// Wait for a client conection
      worker = new std::thread( task, client );
    }
  } else if (mode == 1) {
    std::cout << "Running as server Fork" << std::endl;
    int childpid;
    char a[ BUFSIZE ];
    Socket s1('s'), *s2;
    s1.Bind( PORT );			// Port to access this mirror server
    s1.Listen( 5 );			// Set backlog queue to 5 conections
    for( ; ; ) {
      s2 = s1.Accept();	 		// Wait for a new conection, conection info is in s2 variable
      childpid = fork();		// Create a child to serve the request
      if ( childpid < 0 ) {
        perror( "server: fork error" );
      } else {
        if (0 == childpid) {		// child code
          s1.Close();			// Close original socket "s1" in child
          memset( a, 0, BUFSIZE );
          s2->Read( a, BUFSIZE );	// Read a string from client using new conection info
          s2->Write( a );		// Write it back to client, this is the mirror function
          exit( 0 );			// Exit, finish child work
        }
      }
      s2->Close();			// Close socket s2 in parent, then go wait for a new conection
   }
  } else if (mode == 0) {
    std::cout << "Running as client" << std::endl;
    Socket s('s');     // Crea un socket de IPv4, tipo "stream"
    char buffer[ BUFSIZE ];

    s.Connect( "ip address in dot decimal format", PORT ); // Same port as server
    if ( argc > 1 ) {
        s.Write( argv[1] );		// Send first program argument to server
    } else {
        s.Write( "Hello world 2023 ..." );
    }
    s.Read( buffer, BUFSIZE );	// Read answer sent back from server
    printf( "%s", buffer );	// Print received string
  }

}