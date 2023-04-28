// Copyright 2023 Antonio Badilla Olivas <anthonny.badilla@ucr.ac.cr>.
// based on the code from Francisco Arroyo Mora, 2023, modified based on
// book "The Linux Programming Interface" by Michael Kerrisk, 2010
// chapters 59-61.
#include "Socket.hpp"

int Socket::fdIsValid(int fd) {
  // checks if the file descriptor is valid
  return fcntl(fd, F_GETFD) != -1 || errno != EBADF;
}

Socket::Socket(char SocketType, bool IPv6, bool SSL) {
  // check if socket type is valid.
  if (SocketType != 's' && SocketType != 'd') {
    throw SocketException("Invalid socket type", "Socket::Socket", EINVAL);
  }
  // Set the domain to IPv4 or IPv6
  int domain = AF_INET;
  this->ipv6 = false;
  if (IPv6){
    domain = AF_INET6;
    this->ipv6 = true;
  }
  // Set the socket type to TCP or UDP
  int socketType = 0;
  if (SocketType == 's'){
    socketType = SOCK_STREAM; // TCP socket (connection oriented)
  } else if (SocketType == 'd') {
    socketType = SOCK_DGRAM; // UDP socket (connectionless)
  }
  // Create the socket
  this->idSocket = socket(domain, socketType, 0);
  if (this->idSocket == -1) {
    throw SocketException("Error creating socket", "Socket::Socket", errno);
  }
  // Prepare socket if SSL is enabled
  if (SSL) {
    try {
      this->InitSSLContext();
      this->InitSSL();
    } catch (const SocketException &e) {
      throw_with_nested(SocketException("Error Creating Socket",
        "Socket::Socket", errno));
    }
  } else {
    this->SSLContext = nullptr;
    this->SSLStruct = nullptr;
  }
  this->isOpen = true;
}

Socket::Socket(int socketDescriptor) {
  if (fdIsValid(socketDescriptor) == 0) {
    throw SocketException("Invalid socket descriptor", "Socket::Socket", errno);
  }
  this->idSocket = socketDescriptor;
  this->SSLContext = nullptr;
  this->SSLStruct = nullptr;
}

Socket::~Socket() {
  if (this->isOpen) {
    try {
      this->Close();
    }
    catch (SocketException &e) {
      std::cerr << e.what() << std::endl;
    }
  }
}

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
  this->isOpen = false;
}

void Socket::connectIPv4(const char *host, int port) {
  int st = -1;
  // sockaddr_in is a struct containing an information about internet sockets.
  // sockaddr_in6 is used for IPv6. they contain ip address and port number.
  struct sockaddr_in hostIpv4; // ipv4 address struct set to 0.
  memset(&hostIpv4, 0, sizeof(hostIpv4));
  hostIpv4.sin_family = AF_INET; // socket domain (IPv4)
  // inep_pton(inet_presentation string to network) converts an IP address in
  // dotted-decimal notation to binary form.
  st = inet_pton(AF_INET, host, &hostIpv4.sin_addr);
  if (st == 0) {
    throw SocketException("Invalid IPv4 address", "Socket::Connect", EINVAL);
  } else if (st == -1) {
    throw SocketException("Error converting IPv4 address", "Socket::Connect",
      errno);
  }
  // sin_port is the port number we want to connect to. it is a 16-bit integer
  // network byte order is big endian, host byte order is little endian, so we
  // need to convert the port number to network byte order.
  hostIpv4.sin_port = htons(port); // host to network short (htons)
  struct sockaddr *hostIpv4Ptr = (sockaddr *)&hostIpv4;
  socklen_t hostIpv4Len = sizeof(hostIpv4);
  // connect() system call connects this active socket to a listening socket
  // pasive socket. usually used for TCP sockets.
  st = connect(idSocket, hostIpv4Ptr, hostIpv4Len);
  if (st == -1){
    throw SocketException("Error connecting to IPv4 address", "Socket::Connect",
      errno);
  }
}

void Socket::connectIPv6(const char *host, int port) {
  int st = -1;
  struct sockaddr_in6 hostIpv6;           // IPv6 address struct
  memset(&hostIpv6, 0, sizeof(hostIpv6)); // set to 0
  hostIpv6.sin6_family = AF_INET6;        // socket domain (IPv6)
  // inep_pton(inet_presentation string to network) converts an IP address in
  // dotted-decimal notation to binary form.
  st = inet_pton(AF_INET6, host, &hostIpv6.sin6_addr);
  if (st == 0) {
    throw SocketException("Invalid IPv6 address", "Socket::Connect", EINVAL);
  } else if (st == -1) {
    throw SocketException("Error converting IPv6 address", "Socket::Connect",
      errno);
  }
  // sin_port is the port number we want to connect to. it is a 16-bit integer
  hostIpv6.sin6_port = htons(port);
  struct sockaddr *hostIpv6Ptr = (sockaddr *)&hostIpv6;
  socklen_t hostIpv6Len = sizeof(hostIpv6);
  // connect() system call connects this active socket to a listening socket
  // pasive socket. usually used for TCP sockets.
  st = connect(idSocket, hostIpv6Ptr, hostIpv6Len);
  if (st == -1) {
    throw SocketException("Error connecting to IPv6 address", "Socket::Connect",
      errno);
  }
}

void Socket::Connect(const char* host, int port) {
  try {
    if (this->ipv6) {
      this->connectIPv6(host, port);
    } else {
      this->connectIPv4(host, port);
    }
  } catch (SocketException &e) {
    throw;
  }
}

void Socket::Connect(const char *host, const char *service) {
  int st = -1;
  struct addrinfo hints, *result, *rp;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_UNSPEC;     // to allow IPv4 or IPv6
  hints.ai_socktype = SOCK_STREAM; // TCP
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
    if (0 == st) {
      break;
    }
  }
  freeaddrinfo(result);
  if (st == -1) {
    throw SocketException("Error connecting to host", "Socket::Connect", errno);
  }
}

int Socket::Read(void *buffer, int bufferSize) {
  int nBytesRead = -1;
  // Read from the socket and store the data in buffer using system call read
  nBytesRead = read(this->idSocket, buffer, bufferSize);
  if (-1 == nBytesRead) {
    throw SocketException("Error reading from socket", "Socket::Read", errno);
  } else if (0 == nBytesRead) {
    throw SocketException("Error reading from socket", "Socket::Read",
      ECONNRESET);
  }
  return nBytesRead;
}

void Socket::Write(const void *buffer, int bufferSize){
  int st = -1;
  // Write to the socket using system call write
  st = write(this->idSocket, buffer, bufferSize);
  if (-1 == st) {
    throw SocketException("Error writing to socket", "Socket::Write", errno);
  }
}

void Socket::Write(const char *buffer) {
  try {
    this->Write(buffer, strlen(buffer));
  } catch (SocketException &e) {
    throw;
  }
}

void Socket::Listen(int backlog) {
  int st = -1;
  // mark the socket as passive using system call listen
  st = listen(this->idSocket, backlog);
  if (-1 == st) {
    throw SocketException("Error listening to socket", "Socket::Listen", errno);
  }
}

void Socket::bindIPv4(int port) {
  int st = -1;
  // prepare the address structure for the bind system call
  struct sockaddr_in hostIpv4;
  memset(&hostIpv4, 0, sizeof(hostIpv4));
  hostIpv4.sin_family = AF_INET;         // socket domain is IPv4
  hostIpv4.sin_addr.s_addr = INADDR_ANY; //  bind to any address
  // htons converts the unsigned short integer hostshort from host byte order
  // to network byte order. For portable code, it is recommended to use htons
  // whenver you are using a number from host byte order in a context where
  // network byte order is expected.
  hostIpv4.sin_port = htons(port); //  bind to port
  struct sockaddr *hostIpv4Ptr = (sockaddr *)&hostIpv4;
  socklen_t hostIpv4Len = sizeof(hostIpv4);
  // bind the socket to the address and port number
  st = bind(idSocket, hostIpv4Ptr, hostIpv4Len);
  if (-1 == st) {
    throw SocketException("Error binding to socket IPV4", "Socket::Bind",
      errno);
  }
}

void Socket::bindIPv6(int port) {
  int st = -1;
  // prepare the address structure to bind the socket to an IPv6 address
  struct sockaddr_in6 hostIpv6;
  memset(&hostIpv6, 0, sizeof(hostIpv6));
  hostIpv6.sin6_family = AF_INET6;  // socket domain IPv6
  hostIpv6.sin6_addr = in6addr_any; // bind to any address IPv6
  hostIpv6.sin6_port = htons(port); // port number to bind to
  struct sockaddr *hostIpv6Ptr = (sockaddr *)&hostIpv6;
  socklen_t hostIpv6Len = sizeof(hostIpv6);
  // bind the socket to the address and port number
  st = bind(idSocket, hostIpv6Ptr, hostIpv6Len);
  if (-1 == st) {
    throw SocketException("Error binding to socket IPV6", "Socket::Bind",
      errno);
  }
}

void Socket::Bind(int port) {
  try {
    if (this->ipv6) {
      this->bindIPv6(port);
    } else {
      this->bindIPv4(port);
    }
  } catch (SocketException &e) {
    throw;
  }
}

Socket *Socket::Accept() {
  int newSocketFd;
  // sockaddr_storage is large enough to hold both IPv4 and IPv6 structures
  struct sockaddr_storage clientAddr;
  memset(&clientAddr, 0, sizeof(clientAddr));
  struct sockaddr *clientAddrPtr = (struct sockaddr *)&clientAddr;
  socklen_t clientAddrLen = sizeof(clientAddr);
  // accept a connection on a socket
  newSocketFd = accept(this->idSocket, clientAddrPtr, &clientAddrLen);
  if (newSocketFd < 0) {
    throw SocketException("Error accepting connection", "Socket::Accept",
      errno);
  }
  Socket *newSocket = new Socket(newSocketFd);
  return newSocket;
}

void Socket::Shutdown(int mode) {
  int st = -1;
  // shutdown can be used to disable read, write or both
  st = shutdown(this->idSocket, mode);
  if (-1 == st){
    throw SocketException("Error shutting down socket", "Socket::Shutdown",
      errno);
  }
}

 void Socket::SetIDSocket(int newId) noexcept(true) {
  this->idSocket = newId;
}

int Socket::sendTo(const void *message, int length, const void *destAddr){
  int nBytesSent = -1;
  // Determine the size of the sockaddr structure based on the ipv6 attribute
  socklen_t addrSize = this->ipv6 ? sizeof(sockaddr_in6) : sizeof(sockaddr_in);
  // Send the message using the sendto system call
  nBytesSent = sendto(this->idSocket, message, length, 0, (sockaddr *)destAddr,
    addrSize);
  if (-1 == nBytesSent) {
    throw SocketException("Error sending message", "Socket::sendTo", errno);
  }
  return nBytesSent;
}

int Socket::recvFrom(void *buffer, int length, void *srcAddr) {
  int nBytesReceived = -1;
  // Determine the size of the sockaddr structure based on the ipv6 attribute
  socklen_t addrSize = this->ipv6 ? sizeof(sockaddr_in6) : sizeof(sockaddr_in);
  // Receive data using the recvfrom system call
  nBytesReceived = recvfrom(this->idSocket, buffer, length, 0,
    (sockaddr *)srcAddr, &addrSize);
  if (-1 == nBytesReceived) {
    throw SocketException("Error receiving message", "Socket::recvFrom", errno);
  }
  return nBytesReceived;
}

void Socket::InitSSLContext(){
  // We must create a method to define our context
  const SSL_METHOD *method = TLS_client_method();
  if (method == nullptr) {
    throw SocketException("Error creating SSL method", "Socket::InitSSLContext",
      errno);
  }
  // build a new SSL context using the method
  SSL_CTX *context = SSL_CTX_new(method);
  if (context == nullptr) {
    throw SocketException("Error creating SSL Ctx", "Socket::InitSSLContext",
      errno);
  }
  this->SSLContext = context;
}
/**
 * @brief InitSSLContext method initializes the SSL context
 * @details uses openssl library to initialize the SSL context
 * @throws SocketException if can't create SSL context
 * @throws SocketException if can't create SSL method
 */
void Socket::InitSSL() {
  // Create a SSL socket, a new context must be created before
  try {
    this->InitSSLContext();
  } catch (SocketException &e) {
    throw;
  }
  SSL *ssl = SSL_new(this->SSLContext);
  if (ssl == nullptr) {
    throw SocketException("Error creating SSL", "Socket::InitSSL", errno);
  }
  this->SSLStruct = ssl;
}

void Socket::SSLConnect(const char* host, int port) {
  int st = -1;
  try {
    this->Connect(host, port); // Establish a non SSL connection first
  } catch (SocketException &e) {
    throw_with_nested(SocketException("Error connecting to host",
      "Socket::SSLConnect", errno));
  }
  st = SSL_set_fd(this->SSLStruct, this->idSocket);
  if (-1 == st) {
    throw SocketException("Error setting SSL file descriptor",
      "Socket::SSLConnect", errno);
  }
  st = SSL_connect(this->SSLStruct);
  if (-1 == st) {
    throw SocketException("Error connecting to SSL host", "Socket::SSLConnect",
      errno);
  }
}

void Socket::SSLConnect(const char *host, const char *service){
  int st = -1;
  try {
    this->Connect(host, service); // Establish a non SSL connection first
  } catch (SocketException &e) {
    throw_with_nested(SocketException("Error connecting to host",
      "Socket::SSLConnect", errno));
  }
  st = SSL_set_fd(this->SSLStruct, this->idSocket);
  if (-1 == st) {
    throw SocketException("Error setting SSL file descriptor",
      "Socket::SSLConnect", errno);
  }
  st = SSL_connect(this->SSLStruct);
  if (-1 == st) {
    throw SocketException("Error connecting to SSL host", "Socket::SSLConnect",
      errno);
  }
}

int Socket::SSLRead(void *buffer, int bufferSize){
  int nBytesRead = -1;
  try {
    if (isReadyToRead(this->idSocket, 5)  == false) {
      throw SocketException("Error reading from SSLSocket", "Socket::SSLRead",
        errno);
    }
  } catch (SocketException &e) {
    throw;
  }
  do {
    nBytesRead = SSL_read(this->SSLStruct, buffer, bufferSize);
    if (nBytesRead < 0) {
      int sslError = SSL_get_error(static_cast<SSL *>(this->SSLStruct), 
        nBytesRead);
      if (sslError == SSL_ERROR_WANT_READ || sslError == SSL_ERROR_WANT_WRITE) {
        continue;
      } else {
        throw SocketException("Error reading from SSLSocket", "Socket::SSLRead",
          errno);
      }
    }
  } while (nBytesRead < 0);
  return nBytesRead;
}
/**
 * @brief SSLWrite method uses SSL_write system call to write to a socket
 * @param const void* buffer buffer to store the message
 * @param int bufferSize size of the buffer
 * @return int number of bytes written
 * @throws SocketException if can't write to SSL socket
 */
int Socket::SSLWrite(const void *buffer, int bufferSize) {
  int nBytesWritten = -1;
  // ssl_write returns the number of bytes written or -1 if an error occurs
  nBytesWritten = SSL_write(this->SSLStruct, buffer, bufferSize);
  if (nBytesWritten <= 0) {
    // If the error is SSL_ERROR_WANT_READ it means the write operation was
    // not completed and we must try again. If the error is SSL_ERROR_WANT_WRITE
    // it means the socket is not ready for writing and we must try again.
    int sslError = SSL_get_error(this->SSLStruct, nBytesWritten);
    if (sslError == SSL_ERROR_WANT_READ || sslError == SSL_ERROR_WANT_WRITE) {
      SSLWrite(buffer, bufferSize);
    } else {
      throw SocketException("Error writing to SSL socket", 
        "Socket::SSLWrite", errno);
    }
  }

  return nBytesWritten;
}
bool Socket::isReadyToRead(int timeoutSec, int timeoutMicroSec) {
  // Declare a set of file descriptors to monitor for reading.
  fd_set readSet;
  // Initialize the set to have zero bits for all file descriptors.
  FD_ZERO(&readSet);
  // Add the socket's file descriptor to the set.
  FD_SET(this->idSocket, &readSet);
  // Declare a timeval structure to specify the timeout.
  timeval timeout;
  // Set the timeval structure's seconds field to the input timeoutSec.
  timeout.tv_sec = timeoutSec;
  // Set the timeval structure's microseconds field to the input timeoutMicroSec.
  timeout.tv_usec = timeoutMicroSec;
  // Call the select() syscall to check file descriptor set for readability.
  // It returns the number of ready file descriptors, or -1 if an error.
  int st = select(this->idSocket + 1, &readSet, nullptr, nullptr, &timeout);
  // Check if the select() function returned -1 (indicating an error).
  if (-1 == st){
    // If an error occurred, throw SocketException with description of error,
    // the function name, and the error number.
    throw SocketException("Error checking if socket is ready to read",
      "Socket::isReadyToRead", errno);
  }
  // Return true if the select() function indicated that the socket's fd
  // is ready for reading, otherwise return false.
  return (st > 0 && FD_ISSET(this->idSocket, &readSet));
}