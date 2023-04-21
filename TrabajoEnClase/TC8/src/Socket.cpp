#include "Socket.hpp"
/**
 * @brief Class constructor for sys/socket wrapper 
 * @param	char type: socket type to define ('s' for stream 'd' for datagram)
 *  Stream sockets (SOCK_STREAM) provide a reliable, bidirectional, byte-stream 
 *  communication channel between two endpoints (connection oriented).
 *  Datagram sockets (SOCK_DGRAM) provide unreliable, connectionless,
 *  message-oriented communication.
 *@param	bool ipv6: if we need a IPv6 socket
 */
Socket::Socket(char SocketType, bool IPv6) {
  // Set the domain to IPv4 or IPv6
  int domain = AF_INET;
  this->ipv6 = false;
  if (IPv6) {
    domain = AF_INET6;
    this->ipv6 = true;
  }
  // Set the socket type to TCP or UDP
  int socketType = 0;
  if (SocketType == 's'){
    socketType = SOCK_STREAM;  // TCP socket (connection oriented)
  } else if (SocketType == 'd') {
    socketType = SOCK_DGRAM;  // UDP socket (connectionless)
  }
  // Create the socket
  this->idSocket = socket(domain, socketType, 0);
  this->port = 80;
  this->SSLContext = nullptr;
  this->SSLStruct = nullptr;

  if (this->idSocket < 0) {
    perror("Socket::Socket");
    exit(1);
  }
}
/**
 * @brief constructor for socket, using existing socket descriptor.
 * @param int socketDescriptor
 * @details used for accepting connections, used by the accept method.
*/
Socket::Socket(int socketDescriptor) {
  this->idSocket = socketDescriptor;
}
/**
 * @brief default constructor
 * @details closes socket file descriptor and frees SSL context and structure
 */
Socket::~Socket(){
  // close the socket
  Close();
}
/**
 * @brief Close method uses "close" Unix system call
 */
void Socket::Close(){
  close(this->idSocket);  
  if (this->SSLContext == nullptr) SSL_CTX_free(this->SSLContext);
  if (this->SSLStruct == nullptr) SSL_free(this->SSLStruct );  
}

/**
 * @brief system call connects this active socket to a listening socket.
 *  usually used for TCP sockets.
 * @details the connect() system call serves a purpose when applied to datagram 
 *  sockets. Calling connect() on a datagram socket causes the kernel to record
 *  a particular address as this socket’s peer. For more details on this see: 
 *  The Linux Programming Interface, Michael Kerrisk, Chapter 56 section 6.2
 * @param	char* host host address in dot notation
 * @param	int port port number
 * @return int 0 if success, -1 if error
*/
int Socket::Connect(const char* host, int port) {
  int st;
  struct sockaddr_in host4;
  struct sockaddr * ha;
  memset((char*) &host4, 0, sizeof(host4));
  host4.sin_family = AF_INET;
  inet_pton(AF_INET, host, &host4.sin_addr);
  host4.sin_port = htons(port);
  ha = (sockaddr*) &host4;
  st = connect(idSocket, (sockaddr*) ha, sizeof(host4));
  if (-1 == st) {
    perror("Socket::Connect IPv4 error");
  }
  return st;
}
/**
 * @brief connects with a pasive socket (TCP). It uses getaddrinfo to get the
 * address of the host and then connects to it and hints to specify the type of
 * socket we want to connect to.
 * @param char* host host address in dot notation, example .
 * @param char* service service name
 * @return int 0 if success, -1 if error
*/
int Socket::Connect(const char* host, const char* service) {
  int st = -1;
  struct addrinfo hints, *result, *rp;
  memset(&hints, 0, sizeof(struct addrinfo));
  memset(&result, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;  // TCP
  hints.ai_flags = 0;
  hints.ai_protocol = 0;         
  st = getaddrinfo(host, service, &hints, &result);
  for (rp = result; rp; rp = rp->ai_next) {
    st = connect(this->idSocket, rp->ai_addr, rp->ai_addrlen);
    if ( 0 == st )
      break;
  }
  freeaddrinfo(result);
  if (-1 == st) {
    perror("Socket::Connect IPv6 error");
  }
  return st;
}
/**
 * @brief read method uses read system call to read data from a TCP socket
 * (STREAM). Other system like send/recv could be used for this too.
 * @param void* buffer buffer to store data read from socket
 * @param int size buffer capacity, number of bytes to read
 * @return int number of bytes read
 */
int Socket::Read(void* buffer, int bufferSize) {
  int st = -1; 
  // Read from the socket and store the data in buffer using system call read
  st = read(this->idSocket, buffer, bufferSize);

  if (-1 == st) {
    perror("Socket::Read error");
  }
  return st;
}
/**
 * @brief write method uses write system call.
 * @param const void* buffer to write in.
 * @param int bufferSize the capacity of the buffer.
 * @returns 0 on success, exit(2) otherwise.
*/
int Socket::Write(const void* buffer, int bufferSize) {
  int st = -1;
  // Write to the socket using system call write
  st = write(this->idSocket, buffer, bufferSize);

  if (-1 == st) {
    perror("Socket::Write error");
    exit(2);
  }
  return st;

}
/**
 * @brief write method uses write system call to perform a write operation in a
 *  TCP socket (STREAM). Other system like send/recv could be used for this too.
 * @param const char* buffer to write in.
 * @returns 0 on success, exit(2) otherwise.
*/
int Socket::Write(const char* buffer) {
  int st = -1;
  st = Write(buffer, strlen(buffer));
  if (-1 == st) {
    perror("Socket::Write");
    exit(2);
  }
  return st;
}

/**
 * @brief listen method uses listen system call to mark a socket as passive
 * @details the socket will be used to accept incoming connection requests.
 *  also, no socket that has used connect becaused it is a active socket
 * @param Socket* socket socket to accept
 * @return int 0 if success, -1 if error
 */
int Socket::Listen(int queue) {
  int st = -1;
  st = listen(this->idSocket, queue);
  if (-1 == st) {
    perror("Socket::Listen error");
  }
  return st;
}

/**
 * @brief bind method uses bind system call used to bind a socket to a address.
 *  ussually a well known address is used.
 * @param int port port number
 * @return int 0 if success, -1 if error
*/
int Socket::Bind(int port) {
  int st = -1;
  struct sockaddr* ha;
  struct sockaddr_in host4;
  struct sockaddr_in6 host6;
  // use bind to link the socket to a port
  if (this->ipv6 == false) {
    memset((char*) &host4, 0, sizeof(host4));
    host4.sin_family = AF_INET;
    host4.sin_addr.s_addr = INADDR_ANY;
    host4.sin_port = htons(port);
    ha = (sockaddr*) &host4;
    st = bind(idSocket, ha, sizeof(host4));
  } else {
    memset((char*) &host6, 0, sizeof(host6));
    host6.sin6_family = AF_INET6;
    host6.sin6_addr = in6addr_any;
    host6.sin6_port = htons(port);
    ha = (sockaddr*) &host6;
    st = bind(idSocket, ha, sizeof(host6));
  }
  return st;
}
/**
 * @brief accept method uses accept system call to accepts an incoming
 *  connection on a listening stream socket.
 * @details blocking call, it will wait until a connection is available.
 * @returns a new socket (handle) to communicate with the client.
*/
Socket* Socket::Accept(){
  int new_socket_fd;
  if (this->ipv6 == false) {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    new_socket_fd = accept(this->idSocket, (struct sockaddr*)&client_addr,
      &client_addr_len);
  } else {
    struct sockaddr_in6 client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    new_socket_fd = accept(this->idSocket, (struct sockaddr*)&client_addr,
      &client_addr_len);
  }
  if (new_socket_fd < 0) {
    perror("Socket::Accept");
    exit(2);
  }

  Socket *new_socket = new Socket(new_socket_fd);
  return new_socket;
}
/**
 * @brief shutdown method uses shutdown system call
 * @param int mode mode to shutdown
 *  mode = 0: shutdown read, mode = 1: shutdown write,
 *  mode = 2: shutdown read and write
 * @return int 0 if success, -1 if error
*/
int Socket::Shutdown(int mode) {
  int st = -1;
  // TODO: implement shutdown method
  return st;
}
/**
 * @brief: sets the id of the socket (socket file descriptor)
*/
void Socket::SetIDSocket(int id){
  this->idSocket = id;
}
/**
 * @brief sendTo method uses sendto system call to send a message to a
 *  UDP Socket (datagram)
 * @param const void* message message to send
 * @param int length length of the message
 * @param const void* other socket to send the message to
*/
int Socket::sendTo(const void* message, int length, const void* other) {
  // use boolean ipv6 to determine the size of sockaddr structure
  int st = -1;
  // sentTo using systme call sendto
  if (this->ipv6 == false) {
    st = sendto(this->idSocket, message, length, 0, (sockaddr*) other,
        sizeof(sockaddr_in));
  } else {
    st = sendto(this->idSocket, message, length, 0, (sockaddr*) other,
        sizeof(sockaddr_in6));
  }
  if (-1 == st) {
    perror("Socket::sentTo");
    exit(2);
  }
  return st;
}
/**
 * @brief recvFrom method uses recvfrom system call to receive a message from a
 *  UDP Socket (datagram)
 * @param void* buffer buffer to store the message
 * @param int length length of the message
 * @param void* other socket to receive the message from
 * @return int number of bytes received
*/
int Socket::recvFrom(void* buffer, int length, void* other) {
  // use boolean ipv6 to determine the size of sockaddr structure
  int st = -1;
  int size = 0;
  if (this->ipv6 == false) {
    size = sizeof(sockaddr_in);
    st = recvfrom(this->idSocket, buffer, length, 0, (sockaddr*) other, 
        (socklen_t *) &size);
  } else {
    size = sizeof(sockaddr_in6 );
    st = recvfrom(this->idSocket, buffer, length, 0, (sockaddr*) other, 
        (socklen_t*) &size);
  }
  if (-1 == st) {
    perror("Socket::recvFrom");
    exit(2);
  }
  return st;
} 
/**
 * @brief InitSSLContext method initializes the SSL context
 * @details uses openssl library to initialize the SSL context
 * @return  int 0 if success, -1 if error
*/
int Socket::InitSSL() {
  // Create a SSL socket, a new context must be created before
  this->InitSSLContext();
  SSL* ssl = SSL_new(this->SSLContext);

  if (ssl == nullptr) {
    perror("Socket::InitSSL");
    exit(2);
  }
  this->SSLStruct = ssl;
  return 0;
}
/**
 * @brief SSLConnect method uses SSL_connect system call to connect to a server
 * @param const char* host host name
 * @param int port port number
*/
int Socket::SSLConnect(const char * host, int port) {
  int st = -1;
  Connect(host, port);  // Establish a non SSL connection first
  SSL_set_fd(this->SSLStruct, this->idSocket);
  st = SSL_connect(this->SSLStruct);
  if (-1 == st) {
    perror("Socket::SSLConnect");
    exit(2);
  }
}

/**
 * @brief SSLConnect method uses SSL_connect system call to connect to a server
 * @param const char* host host name
 * @param const char* service service name
 * @details service name can be a port number or a service name
 * @return int 0 if success, -1 if error
*/
int Socket::SSLConnect(const char * host, const char* service) {
  int st = -1;
  Connect(host, service);
  SSL_set_fd(this->SSLStruct, this->idSocket);
  st = SSL_connect(this->SSLStruct);

  if (-1 == st) {
    perror("Socket::SSLConnect");
    exit(2);
  }
}
/**
 * @brief: SSLRead method uses SSL_read system call to read from a socket
 * @param: void* buffer buffer to store the message
 * @param: int bufferSize size of the buffer
 * @return: int number of bytes read
*/
int Socket::SSLRead(void* buffer, int bufferSize) {
  int st = -1;
  st = SSL_read(this->SSLStruct, buffer, bufferSize);

  if ( -1 == st ) { 
    perror("Socket::SSLRead");
    exit(2);
  }
  return st;
}
/**
 * @brief SSLWrite method uses SSL_write system call to write to a socket
 * @param const void* buffer buffer to store the message
 * @param int bufferSize size of the buffer
 * @return int number of bytes written
*/
int Socket::SSLWrite(const void * buffer, int bufferSize) {
  int st = -1;
  st = SSL_write(this->SSLStruct, buffer, bufferSize);

  if (-1 == st) {
    perror("Socket::SSLWrite");
    exit(2);
  }
  return st;
}
/**
 * @brief InitSSLContext method initializes the SSL context
 * @details uses the TLS_client_method to create a new context
 * @return int 0 if success, -1 if error
*/
int Socket::InitSSLContext() {  
  // We must create a method to define our context
  const SSL_METHOD* method = TLS_client_method();
  if (method == nullptr) {
    perror("Socket::InitSSLContext");
    exit(2);
  }
  SSL_CTX* context = SSL_CTX_new(method);
  this->SSLContext = context;
  return 0;
}
