// Copyright 2023 Antonio Badilla Olivas <anthonny.badilla@ucr.ac.cr>.
// based on the code from Francisco Arroyo Mora, 2023, modified based on
// book "The Linux Programming Interface" by Michael Kerrisk, 2010
// chapters 59-61.
#include "Socket.hpp"
/**
 * @brief Checks if the given file descriptor is valid or not.
 *
 * @param fd The file descriptor to be checked for validity.
 * @return 1 if the file descriptor is valid or there's an error other than a
 *  bad file descriptor, and 0 if the file descriptor is invalid.
 */
int Socket::fdIsValid(int fd) {
  return fcntl(fd, F_GETFD) != -1 || errno != EBADF;
} 
/**
 * @brief Class constructor for sys/socket wrapper 
 * @param	char type: socket type to define ('s' for stream 'd' for datagram)
 *  Stream sockets (SOCK_STREAM) provide a reliable, bidirectional, byte-stream 
 *  communication channel between two endpoints (connection oriented).
 *  Datagram sockets (SOCK_DGRAM) provide unreliable, connectionless,
 *  message-oriented communication.
 * @param	bool ipv6: if we need a IPv6 socket
 * @throws SocketException if the socket type is invalid.
 * @throws SocketException if the socket can't be created.
 */
Socket::Socket(char SocketType, bool IPv6) {
  // check if socket type is valid.
  if (SocketType != 's' && SocketType != 'd') {
    throw SocketException("Invalid socket type", "Socket::Socket", EINVAL);
  }
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
  if (this->idSocket == -1) {
    throw SocketException("Error creating socket", "Socket::Socket", errno);
  }
  this->SSLContext = nullptr;
  this->SSLStruct = nullptr;
}
/**
 * @brief constructor for socket, using existing socket descriptor.
 * @param int socketDescriptor
 * @details used for accepting connections, used by the accept method.
 * @throws SocketException if the socket descriptor is invalid.
*/
Socket::Socket(int socketDescriptor) {
  if (fdIsValid(socketDescriptor) == 0) {
    throw SocketException("Invalid socket descriptor", "Socket::Socket", errno);
  }
  this->idSocket = socketDescriptor;
  this->SSLContext = nullptr;
  this->SSLStruct = nullptr;
}
/**
 * @brief default constructor
 * @details closes socket file descriptor and frees SSL context and structure
 * 
 */
Socket::~Socket() {
  try {
    this->Close();
  } catch (SocketException& e) {
    std::cerr << e.what() << std::endl;
  }
}
/**
 * @brief Close method uses "close" Unix system call. it closes the socket file
 * descriptor and frees SSL context and structure if they exist.
 * @throws SocketException if the socket can't be closed.
 */
void Socket::Close() {
  int st = close(this->idSocket);  
  if (st == -1) {
    throw SocketException("Error closing socket", "Socket::Close", errno);
  }
  if (this->SSLContext != nullptr) {
    SSL_CTX_free(this->SSLContext);
  }
  if (this->SSLStruct != nullptr) {
    SSL_free(this->SSLStruct);
  }  
}
/**
 * @brief Connects to an IPv4 host at the specified port number.
 *
 * @param host The IPv4 address of the host in dotted-decimal notation.
 * @param port The port number to connect to.
 * @throws SocketException If there is an error connecting to the host.
 * @throws SocketException If the IPv4 address is invalid.
 */
void Socket::connectIPv4(const char* host, int port) {
  int st = -1;
  // sockaddr_in is a struct containing an information about internet sockets.
  // sockaddr_in6 is used for IPv6. they contain ip address and port number.
  struct sockaddr_in host4 = {};  // ipv4 address struct set to 0.
  struct sockaddr * ha;
  host4.sin_family = AF_INET;  // socket domain (IPv4)
  // inep_pton(inet_presentation string to network) converts an IP address in
  // dotted-decimal notation to binary form.
  st = inet_pton(AF_INET, host, &host4.sin_addr);
  if (st == 0) {
    throw SocketException("Invalid IPv4 address", "Socket::Connect", EINVAL);
  } else if (st == -1) {
    throw SocketException("Error converting IPv4 address", "Socket::Connect",
                          errno);
  }
  // sin_port is the port number we want to connect to. it is a 16-bit integer
  // network byte order is big endian, host byte order is little endian, so we
  // need to convert the port number to network byte order.
  host4.sin_port = htons(port);  // host to network short (htons)
  ha = (sockaddr*) &host4;
  // connect() system call connects this active socket to a listening socket
  // pasive socket. usually used for TCP sockets.
  st = connect(idSocket, (sockaddr*) ha, sizeof(host4));
  if (st == -1) {
    throw SocketException("Error connecting to IPv4 address", "Socket::Connect",
                          errno);
  }
}
/**
 * @brief Connects to an IPv6 host at the specified port number.
 *
 * @param host The IPv6 address of the host in dotted-decimal notation.
 * @param port The port number to connect to.
 * @throws SocketException If there is an error connecting to the host.
 * @throws SocketException If the IPv6 address is invalid.
 */
void Socket::connectIPv6(const char* host, int port) {
  int st = -1;
  struct sockaddr_in6 host6 = {};  // IPv6 address struct set to 0
  struct sockaddr * ha;
  host6.sin6_family = AF_INET6;  // socket domain (IPv6)
  // inep_pton(inet_presentation string to network) converts an IP address in
  // dotted-decimal notation to binary form.
  st = inet_pton(AF_INET6, host, &host6.sin6_addr);
  if (st == 0) {
    throw SocketException("Invalid IPv6 address", "Socket::Connect", EINVAL);
  } else if (st == -1) {
    throw SocketException("Error converting IPv6 address", "Socket::Connect",
                          errno);
  }
  // sin_port is the port number we want to connect to. it is a 16-bit integer
  host6.sin6_port = htons(port);
  ha = (sockaddr*) &host6;
  // connect() system call connects this active socket to a listening socket
  // pasive socket. usually used for TCP sockets.
  st = connect(idSocket, (sockaddr*) ha, sizeof(host6));
  if (st == -1) {
    throw SocketException("Error connecting to IPv6 address", "Socket::Connect",
                          errno);
  }
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
 * @throws with nested SocketException if: can't connect to host.
*/
void Socket::Connect(const char* host, int port) {
  try {
    if (this->ipv6) {
      this->connectIPv6(host, port);
    } else {
      this->connectIPv4(host, port);
    }
  } catch (SocketException& e) {
    throw;
  }
}
/**
 * @brief connects with a pasive socket (TCP). It uses getaddrinfo to get the
 * address of the host and then connects to it and hints to specify the type of
 * socket we want to connect to.
 * @param char* host host address in dot notation, example .
 * @param char* service service name
 * @throws SocketException if can't get address info
 * @throws SocketException if can't connect to host
*/
void Socket::Connect(const char* host, const char* service) {
  int st = -1;
  struct addrinfo hints, *result, *rp;
  memset(&hints, 0, sizeof(struct addrinfo));
  memset(&result, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC; // to allow IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM;  // TCP
  hints.ai_flags = 0;
  hints.ai_protocol = 0; 
  hints.ai_canonname = nullptr;
  hints.ai_addr = nullptr;
  hints.ai_next = nullptr;
  // Given a hostname and a service name, getaddrinfo() returns a set of
  // structures containing the corresponding binary IP address(es) and port 
  // number. Thats why we use rp and result to iterate over the list of 
  // addresses returned by getaddrinfo.
  st = getaddrinfo(host, service, &hints, &result);
  if (st != 0) {
    throw SocketException("Error getting address info", "Socket::Connect", st);
  }
  for (rp = result; rp; rp = rp->ai_next) {
    st = connect(this->idSocket, rp->ai_addr, rp->ai_addrlen);
    if (0 == st)
      break;
  }
  freeaddrinfo(result);
  if (st == -1) {
    throw SocketException("Error connecting to host", "Socket::Connect", errno);
  }
}
/**
 * @brief read method uses read system call to read data from a TCP socket
 * (STREAM). Other system like send/recv could be used for this too.
 * @param void* buffer buffer to store data read from socket
 * @param int size buffer capacity, number of bytes to read
 * @throws SocketException if can't read from socket
 * @throws SocketException if connection was closed by peer
 * @return int number of bytes read
 */
int Socket::Read(void* buffer, int bufferSize) {
  int st = -1; 
  // Read from the socket and store the data in buffer using system call read
  st = read(this->idSocket, buffer, bufferSize);
  if (-1 == st) {
    throw SocketException("Error reading from socket", "Socket::Read", errno);
  } else if (0 == st) {
    throw SocketException("Error reading from socket", "Socket::Read",
                          ECONNRESET);
  }
  return st;
}
/**
 * @brief write method uses write system call.
 * @param const void* buffer to write in.
 * @param int bufferSize the capacity of the buffer.
 * @throws SocketException if can't write to socket
*/
void Socket::Write(const void* buffer, int bufferSize) {
  int st = -1;
  // Write to the socket using system call write
  st = write(this->idSocket, buffer, bufferSize);
  if (-1 == st) {
    throw SocketException("Error writing to socket", "Socket::Write", errno);
  }
}
/**
 * @brief write method uses write system call to perform a write operation in a
 *  TCP socket (STREAM). Other system like send/recv could be used for this too.
 * @param const char* buffer to write in.
 * @throws SocketException if can't write to socket
*/
void Socket::Write(const char* buffer) {
 try {
    this->Write(buffer, strlen(buffer));
  } catch (SocketException& e) {
    throw;
  }
}

/**
 * @brief listen method uses listen system call to mark a socket as passive
 * @details the socket will be used to accept incoming connection requests.
 *  also, no socket that has used connect because it is an active socket
 * @param int backlog maximum number of pending connection requests in the queue
 * @throws SocketException if can't listen to socket
 */
void Socket::Listen(int backlog) {

  int st = -1;
  st = listen(this->idSocket, backlog);
  if (-1 == st) {
    throw SocketException("Error listening to socket", "Socket::Listen", errno);
  }
}
/**
 * @brief Binds the socket to an IPv4 address and port number.
 *
 * @param port The port number to bind to.
 * @throws SocketException If there is an error binding to the socket.
 */
void Socket::bindIPv4(int port) {
  int st = -1;
  // prepare the address structure for the bind system call
  struct sockaddr* ha;
  struct sockaddr_in hostIpv4 = {};
  hostIpv4.sin_family = AF_INET;  // socket domain is IPv4
  hostIpv4.sin_addr.s_addr = INADDR_ANY;  //  bind to any address
  // htons converts the unsigned short integer hostshort from host byte order
  // to network byte order. For portable code, it is recommended to use htons 
  // whenver you are using a number from host byte order in a context where
  // network byte order is expected.
  hostIpv4.sin_port = htons(port);  //  bind to port
  ha = (sockaddr*) &hostIpv4;
  st = bind(idSocket, ha, sizeof(hostIpv4));
  if (-1 == st) {
    throw SocketException("Error binding to socket IPV4", "Socket::Bind",
                          errno);
  }
}
/**
 * @brief Binds the socket to an IPv6 address and port number.
 * @param port The port number to bind to.
 * @throws SocketException If there is an error binding to the socket.
 */
void Socket::bindIPv6(int port) {
  int st = -1;
  // prepare the address structure to bind the socket to an IPv6 address
  struct sockaddr* ha;
  struct sockaddr_in6 hostIpv6 = {};
  hostIpv6.sin6_family = AF_INET6;  // socket domain IPv6
  hostIpv6.sin6_addr = in6addr_any;  // bind to any address IPv6
  hostIpv6.sin6_port = htons(port);   // port number to bind to
  ha = (sockaddr*) &hostIpv6;
  st = bind(idSocket, ha, sizeof(hostIpv6));
  if (-1 == st) {
    throw SocketException("Error binding to socket IPV6", "Socket::Bind",
                          errno);
  }
}
/**
 * @brief bind method uses bind system call used to bind a socket to a address.
 *  ussually a well known address is used.
 * @param int port port number
 * @throws SocketException if can't bind to socket
*/
int Socket::Bind(int port) {
  try {
    if (this->ipv6){
      this->bindIPv6(port);
    } else {
      this->bindIPv4(port);
    }
  } catch (SocketException& e) {
    throw;
  }
}
/**
 * @brief accept method uses accept system call to accepts an incoming
 *  connection on a listening stream socket.
 * @details blocking call, it will wait until a connection is available.
 * @returns a new socket (handle) to communicate with the client.
*/
Socket* Socket::Accept(){
  int newSocketFd;
  // sockaddr_storage is large enough to hold both IPv4 and IPv6 structures
  struct sockaddr_storage clientAddr;
  socklen_t clientAddrLen = sizeof(clientAddr);

  newSocketFd = accept(this->idSocket, (struct sockaddr*)&clientAddr,
   &clientAddrLen);

  if (newSocketFd < 0) {
    perror("Socket::Accept");
  }
  Socket *newSocket = new Socket(newSocketFd);
  return newSocket;
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
  st = shutdown(this->idSocket, mode);
  if (st == -1) {	// check for errors
    perror("Socket::Shutdown");
  }
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
 * @param const void* destAddr socket to send the message to
*/
int Socket::sendTo(const void* message, int length, const void* destAddr) {
  int st = -1;
  // Determine the size of the sockaddr structure based on the ipv6 attribute
  int addrSize = this->ipv6 ? sizeof(sockaddr_in6) : sizeof(sockaddr_in);
  // Send the message using the sendto system call
  st = sendto(this->idSocket, message, length, 0, (sockaddr*)destAddr, 
    addrSize);
  if (-1 == st) {
    perror("Socket::sendTo");
  }
  return st;
}

/**
 * @brief recvFrom method uses recvfrom system call to receive a message from a
 *  UDP Socket (datagram)
 * @param void* buffer buffer to store the message
 * @param int length length of the message
 * @param void* srcAddr socket to receive the message from
 * @return int number of bytes received
*/
int Socket::recvFrom(void* buffer, int length, void* srcAddr) {
  int st = -1;
  // Determine the size of the sockaddr structure based on the ipv6 attribute
  int addrSize = this->ipv6 ? sizeof(sockaddr_in6) : sizeof(sockaddr_in);
  // Receive data using the recvfrom system call
  st = recvfrom(this->idSocket, buffer, length, 0, (sockaddr*)srcAddr,
    (socklen_t*)&addrSize);
  if (-1 == st) {
    perror("Socket::recvFrom");
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
  }
  return st;
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
  }
  return st;
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
  }
  SSL_CTX* context = SSL_CTX_new(method);
  this->SSLContext = context;
  return 0;
}