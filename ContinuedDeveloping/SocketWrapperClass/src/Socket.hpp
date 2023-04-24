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
  int Bind(int port) noexcept(false);
  Socket* Accept() noexcept(false);
  void Shutdown(int mode) noexcept(false);
  void SetIDSocket(int newId) noexcept(true);
  int sendTo(const void* message, int length, const void* destAddr) 
    noexcept(false);
  int recvFrom(void* buffer, int length, void* srcAddr) noexcept(false);
  void SSLConnect(const char* host, int port) noexcept(false);
  void SSLConnect(const char* host, const char * service) noexcept(false);
  int SSLRead(void* buffer, int bufferSize) noexcept(false);
  int SSLWrite(const void* buffer, int bufferSize) noexcept(false);
  void InitSSLContext() noexcept(false);
  void InitSSL() noexcept(false);

 private:
  int idSocket;
  int port;
  bool ipv6;
  SSL_CTX *SSLContext;
  SSL *SSLStruct;
  int fdIsValid(int fd);
  void connectIPv4(const char* host, int port) noexcept(false);
  void connectIPv6(const char* host, int port) noexcept(false);
  void bindIPv4(int port) noexcept(false);
  void bindIPv6(int port) noexcept(false);
};
#endif
