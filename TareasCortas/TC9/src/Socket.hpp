// TODO (Antonio): modify class to handle errors by throwing exceptions
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#ifndef SOCKET_HPP
#define SOCKET_HPP

class Socket {
 public:
  Socket(char SocketType, bool IPv6 = false);
  Socket(int socketDescriptor);
  ~Socket();
  int Connect(const char* host, int port);
  int Connect(const char* host, const char* service);
  void Close();
  int Read(void* buffer, int bufferSize);
  int Write(const void* buffer, int bufferSize);
  int Write(const char* buffer);
  int Listen(int backlog);
  int Bind(int port);
  Socket* Accept();
  int Shutdown(int mode);
  void SetIDSocket(int newId);
  int sendTo(const void* message, int length, const void* destAddr);
  int recvFrom(void* buffer, int length, void* srcAddr);
  int SSLConnect(const char* host, int port);
  int SSLConnect(const char* host, const char * service);
  int SSLRead(void* buffer, int bufferSize);
  int SSLWrite(const void* buffer, int bufferSize);
  int InitSSLContext();
  int InitSSL();

 private:
  int idSocket;
  int port;
  bool ipv6;
  SSL_CTX *SSLContext;
  SSL *SSLStruct;

};
#endif
