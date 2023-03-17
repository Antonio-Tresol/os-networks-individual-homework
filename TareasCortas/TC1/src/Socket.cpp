/**
 *   CI0123 PIRO
 *   Clase para utilizar los "sockets" en Linux
 *
 **/

#include <stdio.h>	// for perror
#include <stdlib.h>	// for exit
#include <string.h>	// for memset
#include <arpa/inet.h>	// for inet_pton
#include <sys/types.h>	// for connect 
#include <unistd.h>   // for close
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include "Socket.hpp"

/**
  *  @brief Class constructor
  *     use Unix socket system call
  *  @param	char type: socket type to define
  *     's' for stream
  *     'd' for datagram
  *  @param	bool ipv6: if we need a IPv6 socket
  *
 **/
Socket::Socket(char type, bool IPv6 ) {
   // Set the domain to IPv4 or IPv6
   int domain = AF_INET;
   this->ipv6 = false;
   if (IPv6) {
      domain = AF_INET6;
      this->ipv6 = true;
   }
   // Set the socket type to TCP or UDP
   int socketType = 0;
   if (type == 's'){
      socketType = SOCK_STREAM;
   } else if (type == 'd') {
      socketType = SOCK_DGRAM;
   }
   // Create the socket
   this->idSocket = socket(domain, socketType, 0);
   this->port = 80;
   // Handle errors
   if (this->idSocket < 0) {
      // Error handling
      perror("Socket::Socket");
      exit(1);
   }

}

/**
  * Class destructor
  *   use Unix close system call (once opened a socket is managed like a file in Unix)
  *  close the socket
  * 
 **/
Socket::~Socket(){
    // close the socket
    Close();
}

/**
 * @brief Close method use Unix close system call 
 * (once opened a socket is managed like a file in Unix)
 **/
void Socket::Close(){
   close(this->idSocket);  
}

/**
  * @brief Connect method uses "connect" Unix system call
  * @param	char * host: host address in dot notation, example "10.1.104.187"
  * @param	int port: process address, example 80
  *
 **/
int Socket::Connect( const char * host, int port ) {
   int st;
   struct sockaddr_in host4;
   struct sockaddr * ha;
   memset( (char *) &host4, 0, sizeof( host4 ) );
   host4.sin_family = AF_INET;
   inet_pton( AF_INET, host, &host4.sin_addr );
   host4.sin_port = htons( port );
   ha = (sockaddr*) &host4;
   st = connect( idSocket, (sockaddr *) ha, sizeof( host4 ) );
   if ( -1 == st ) {	// check for errors
      perror( "Socket::Connect IPv4" );
      exit( 2 );
   }
   return st;
}

/**
  * Connect method
  *
  * @param	char * host: host address in dot notation, example "163.178.104.187"
  * @param	char * service: service name, example "http"
  *
 **/
int Socket::Connect( const char * host, const char * service ) {
   int st = -1;
   struct addrinfo hints, *result, *rp;
   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
   hints.ai_socktype = SOCK_STREAM; /* Stream socket */
   hints.ai_flags = 0;
   hints.ai_protocol = 0;          /* Any protocol */
   st = getaddrinfo( host, service, &hints, &result );
   for ( rp = result; rp; rp = rp->ai_next ) {
      st = connect( idSocket, rp->ai_addr, rp->ai_addrlen );
      if ( 0 == st )
         break;
   }
   freeaddrinfo( result );
   if ( -1 == st ) {	// check for errors
      perror( "Socket::Connect IPv4" );
      exit( 2 );
   }
   return st;
}

/**
  * Read method
  *   use "read" Unix system call (man 3 read)
  *
  * @param	void * text: buffer to store data read from socket
  * @param	int size: buffer capacity, read will stop if buffer is full
  *
 **/
int Socket::Read( void * buffer, int bufferSize ) {
   int st = -1; 
   // Read from the socket and store the data in buffer using system call read
   st = read(this->idSocket, buffer, bufferSize);
   // check for errors
   if ( -1 == st ) {	// check for errors
      perror( "Socket::Read" );
      exit( 2 );
   }
   return st;

}

/**
  * Write method
  *   use "write" Unix system call (man 3 write)
  *
  * @param	void * buffer: buffer to store data write to socket
  * @param	size_t size: buffer capacity, number of bytes to write
  *
 **/
int Socket::Write( const void * buffer, int bufferSize ) {
   int st = -1;
   // Write to the socket using system call write
   st = write(this->idSocket, buffer, bufferSize);
   // check for errors
   if ( -1 == st ) {	// check for errors
      perror( "Socket::Write" );
      exit( 2 );
   }
   return st;

}

/**
  * Write method
  *
  * @param	char * text: string to store data write to socket
  *
  *  This method write a string to socket, use strlen to determine how many bytes
  *
 **/
int Socket::Write( const char *buffer ) {
   int st = -1;
   st = Write(buffer, strlen(buffer));
   if ( -1 == st ) {	// check for errors
      perror( "Socket::Write" );
      exit( 2 );
   }
   return st;
}


/**
  * Listen method
  *   use "listen" Unix system call (server mode)
  *
  * @param	int queue: max pending connections to enqueue 
  *
  *  This method define how many elements can wait in queue
  *
 **/
int Socket::Listen( int queue ) {
    int st = -1;

    return st;

}


/**
  * Bind method
  *    use "bind" Unix system call (man 3 bind) (server mode)
  *
  * @param	int port: bind a socket to a port defined in sockaddr structure
  *
  *  Links the calling process to a service at port
  *
 **/
int Socket::Bind( int port ) {
   int st = -1;
   struct sockaddr * ha;
   struct sockaddr_in host4;
   struct sockaddr_in6 host6;

   return st;

}


/**
  * Accept method
  *    use "accept" Unix system call (man 3 accept) (server mode)
  *
  *  @returns	a new class instance
  *
  *  Waits for a new connection to service (TCP  mode: stream)
  *
 **/
Socket * Socket::Accept(){

   return (Socket *) 0;

}


/**
  * Shutdown method
  *    use "shutdown" Unix system call (man 3 shutdown)
  *
  *  @param	int mode define how to cease socket operation
  *
  *  Partial close the connection (TCP mode)
  *
 **/
int Socket::Shutdown( int mode ) {
   int st = -1;

   return st;

}


/**
  *  SetIDSocket
  *
  *  @param	int id assigns a new value to field
  *  
 **/
void Socket::SetIDSocket(int id){

    this->idSocket = id;

}