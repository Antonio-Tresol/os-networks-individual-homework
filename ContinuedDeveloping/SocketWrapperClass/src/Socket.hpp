// Copyright 2023 Antonio Badilla Olivas <anthonny.badilla@ucr.ac.cr>.
// based on the code from Francisco Arroyo Mora, 2023, modified based on
// book "The Linux Programming Interface" by Michael Kerrisk, 2010
// chapeters 59-61.
// TODO (Antonio): modify class to handle errors by throwing exceptions
#include <stdio.h>
#include <iostream>
#include <fcntl.h>
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
#include "SocketException.hpp"

#ifndef SOCKET_HPP
#define SOCKET_HPP

class Socket {
 public:
  Socket(char SocketType, bool IPv6 = false) noexcept(false);
  Socket(int socketDescriptor) noexcept(false);
  ~Socket() noexcept(true);
  void Connect(const char* host, int port) noexcept(false);
  void Connect(const char* host, const char* service) noexcept(false);
  void Close() noexcept(false);
  int Read(void* buffer, int bufferSize) noexcept(false);
  void Write(const void* buffer, int bufferSize) noexcept(false);
  void Write(const char* buffer) noexcept(false);
  void Listen(int backlog) noexcept(false);
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
  int fdIsValid(int fd);
  void connectIPv4(const char* host, int port);
  void connectIPv6(const char* host, int port);
  void bindIPv4(int port) noexcept(false);
  void bindIPv6(int port) noexcept(false);

};
#endif
