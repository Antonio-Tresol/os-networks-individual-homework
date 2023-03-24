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
#include <openssl/ssl.h>
#include <openssl/err.h>

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
   this->SSLContext = nullptr;
   this->SSLStruct = nullptr;

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
   if (this->SSLContext == nullptr) SSL_CTX_free((SSL_CTX *) this->SSLContext);
   if (this->SSLStruct == nullptr) SSL_free((SSL*) this->SSLStruct );
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
      perror( "Socket::Connect IPv4 error" );
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
   memset( &hints, 0, sizeof( struct addrinfo ) );
   memset( &result, 0, sizeof( struct addrinfo ) );
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM; /* Stream socket is  a TCP socket */
   hints.ai_flags = 0;
   hints.ai_protocol = 0;         
   st = getaddrinfo( host, service, &hints, &result );
   for ( rp = result; rp; rp = rp->ai_next ) {
      st = connect( this->idSocket, rp->ai_addr, rp->ai_addrlen );
      if ( 0 == st )
         break;
   }
   freeaddrinfo( result );
   if ( -1 == st ) {	// check for errors
      perror( "Socket::Connect IPv6 error" );
      exit( 2 );
   }
   return st;
}
// int Socket::ConnectIPV6(const char* host, int port) {
//   struct sockaddr_in6 host6;
//   memset(reinterpret_cast<char*>(&host6), 0, sizeof(host6));
//   host6.sin6_family = AF_INET6;
//   inet_pton(AF_INET6, host, &host6.sin6_addr);
//   host6.sin6_port = htons(port);
//   int st = connect(this->idSocket, reinterpret_cast<sockaddr*>(&host6), sizeof(host6));
//   if (st == -1) {
//     perror("Socket::Connect");
//     exit(2);
//   }
//   return st;
// }
// int Socket::ConnectIPV6(const char* host, const char* port) {
//   struct sockaddr_in6 host6;
//   memset(reinterpret_cast<char *>(&host6), 0, sizeof(host6));
//   host6.sin6_family = AF_INET6;
//   inet_pton(AF_INET6, host, &host6.sin6_addr);
//   host6.sin6_port = htons(atoi(port));
//   int st = connect(this->idSocket, reinterpret_cast<sockaddr *>(&host6),
//            sizeof(host6));
//   if (st == -1) {
//     perror("Socket::Connect");
//     exit(2);
//   }
//   return st;
// }

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
      perror( "Socket::Read error" );
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
      perror( "Socket::Write error" );
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
   // use bind to link the socket to a port
   if ( this->ipv6 == false ) {
      memset( (char *) &host4, 0, sizeof( host4 ) );
      host4.sin_family = AF_INET;
      host4.sin_addr.s_addr = INADDR_ANY;
      host4.sin_port = htons( port );
      ha = (sockaddr*) &host4;
      st = bind( idSocket, ha, sizeof( host4 ) );
   } else {
      memset( (char *) &host6, 0, sizeof( host6 ) );
      host6.sin6_family = AF_INET6;
      host6.sin6_addr = in6addr_any;
      host6.sin6_port = htons( port );
      ha = (sockaddr*) &host6;
      st = bind( idSocket, ha, sizeof( host6 ) );
   }
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

int Socket::sendTo( const void * message, int length, const void * other ){
   // use boolean ipv6 to determine the size of sockaddr structure
   int st = -1;
   // sentTo using systme call sendto
   if ( this->ipv6 == false ) {
      st = sendto( this->idSocket, message, length, 0, (sockaddr *) other,
         sizeof( sockaddr_in ) );
   } else {
      st = sendto( this->idSocket, message, length, 0, (sockaddr *) other,
         sizeof( sockaddr_in6 ) );
   }
   // check for errors
   if ( -1 == st ) {	// check for errors
      perror( "Socket::sentTo" );
      exit( 2 );
   }
   return st;
}

int Socket::recvFrom( void * buffer, int length, void * other ){
   // use boolean ipv6 to determine the size of sockaddr structure
   int st = -1;
   int size = 0;
   if ( this->ipv6 == false ) {
      size = sizeof( sockaddr_in );
      st = recvfrom( this->idSocket, buffer, length, 0, (sockaddr *) other, 
         (socklen_t *) &size );
   } else {
      size = sizeof( sockaddr_in6 );
      st = recvfrom( this->idSocket, buffer, length, 0, (sockaddr *) other, 
         (socklen_t *) &size );
   }
   if ( -1 == st ) {	// check for errors
      perror( "Socket::recvFrom" );
      exit( 2 );
   }
   return st;
} 
int Socket::InitSSL(){
   // Create a SSL socket, a new context must be created before
   this->InitSSLContext();
   SSL * ssl = SSL_new( (SSL_CTX *) this->SSLContext );
   // Check for errors
   if ( ssl == nullptr ) {
      perror( "Socket::InitSSL" );
      exit( 2 );
   }
   this->SSLStruct = (void *) ssl;
}

int Socket::SSLConnect(const char * host, int port){
   int st = -1;
   Connect( host, port );	// Establish a non SSL connection first
   SSL_set_fd( (SSL *) this->SSLStruct, this->idSocket );
   st = SSL_connect( (SSL *) this->SSLStruct );
   // check for errors
   if ( -1 == st ) {	// check for errors
      perror( "Socket::SSLConnect" );
      exit( 2 );
   }
}
int Socket::SSLConnect(const char * host, const char * service ){
   int st = -1;
   if (this->ipv6){
      st = Connect(host, service);
   } else {
      st = Connect(host, service);
   }
   SSL_set_fd( (SSL *) this->SSLStruct, this->idSocket );
   st = SSL_connect( (SSL *) this->SSLStruct );
   // check for errors
   if ( -1 == st ) {	// check for errors
     perror( "Socket::SSLConnect" );
     exit( 2 );
   }
}
int Socket::SSLRead(void * buffer, int bufferSize ){
   int st = -1;
   st = SSL_read( (SSL *) this->SSLStruct, buffer, bufferSize );
   // check for errors
   if ( -1 == st ) {	// check for errors
      perror( "Socket::SSLRead" );
      exit( 2 );
   }
   return st;
}
int Socket::SSLWrite(const void * buffer, int bufferSize ){
   int st = -1;
   st = SSL_write( (SSL *) this->SSLStruct, buffer, bufferSize );
   // check for errors
   if ( -1 == st ) {	// check for errors
      perror( "Socket::SSLWrite" );
      exit( 2 );
   }
   return st;
}

int Socket::InitSSLContext(){  
   // We must create a method to define our context
   const SSL_METHOD* method = TLS_client_method();
   // Check for errors
   if ( method == nullptr) {
      perror( "Socket::InitSSLContext" );
      exit( 2 );
   }
   SSL_CTX * context = SSL_CTX_new( method );
   this->SSLContext = (void *) context;
   return 0;
}