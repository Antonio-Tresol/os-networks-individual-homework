#include <iostream>
#include "Socket.hpp"
/// Function to run the server-side TCP code.
void runServer() {
  // Server code
  std::cout << "-ServerTCP- Running as Server" << std::endl;
  try {
    // We are using a TCP socket (SOCK_STREAM) over IPv4
    int port = 8080;
    Socket serverSocket('s');
    // Bind the server socket to a specific port
    serverSocket.Bind(port);
    // Start listening for incoming connections
    serverSocket.Listen(10);
    // Continuously accept incoming connections
    Socket *clientSocket = serverSocket.Accept();
    // Receive data from the client
    char buffer[1024];
    int bytesRead = clientSocket->Read(buffer, sizeof(buffer) - 1);
    buffer[bytesRead] = '\0';
    std::cout << "-ServerTCP- Received: " << buffer << std::endl;
    // Send a response to the client
    clientSocket->Write("Message received");
    // Close the client socket
    clientSocket->Close();
    // Clean up the memory
    delete clientSocket;
    // Close the server socket
    serverSocket.Close();
  }
  catch (const SocketException &e) {
    std::cerr << "Server error: " << e.what() << std::endl;
  }
}

/// Function to run the client-side TCP code.
void runClient() {
  // Client code
  std::cout << "-ClientTCP- Running as Client" << std::endl;
  try {
    // We are using a TCP socket (SOCK_STREAM) over IPv4
    const char *host = "127.0.0.1"; // Change from "localhost" to "127.0.0.1"
    int port = 8080;
    Socket clientSocket('s');
    // Connect to the server at the given host and port
    clientSocket.Connect(host, port);
    // Send a message to the server
    clientSocket.Write("Hello, server!");
    // Receive a response from the server
    char buffer[1024];
    int bytesRead = clientSocket.Read(buffer, sizeof(buffer) - 1);
    buffer[bytesRead] = '\0';
    std::cout << "-ClientTCP- Server response: " << buffer << std::endl;
  }
  catch (const SocketException &e) {
    std::cerr << "Client error: " << e.what() << std::endl;
  }
}

#include <sstream>

// Function to run the SSL client-side code.
void runSslClientIpv4() {
  // Client code
  std::cout << "-ClientSSL- Running as Client IPV4" << std::endl;
  try {
    // We are using a TCP socket (SOCK_STREAM) over IPv4 with SSL
    const char *host = "os.ecci.ucr.ac.cr";
    const char *service = "https";
    Socket clientSocket('s', false, true);
    // Connect to the server using SSL at the given host and service
    clientSocket.SSLConnect(host, service);
    // Create and send an HTTP GET request
    std::ostringstream httpRequest;
    httpRequest << "GET /lego/list.php?figure=giraffe HTTP/1.1\r\n";
    httpRequest << "Host: " << host << "\r\n";
    httpRequest << "Connection: close\r\n";
    httpRequest << "\r\n";
    clientSocket.SSLWrite(httpRequest.str().c_str(), httpRequest.str().length());
    // Receive the HTTP response from the server
    char buffer[1024];
    int bytesRead;
    while ((bytesRead = clientSocket.SSLRead(buffer, sizeof(buffer) - 1)) > 0) {
      buffer[bytesRead] = '\0';
      std::cout << buffer;
    }
    // Close the connection
    clientSocket.Close();
  }
  catch (const SocketException &e) {
    std::cerr << "SSL Client error: " << e.what() << std::endl;
  }
}

// Function to run the SSL client-side code with IPv6.
//  DISCLAIMER: This function will only work if the server supports IPv6 and
//  computer running this code has IPv6 enabled.
void runSslClientIpv6() {
  // Client code
  std::cout << "-ClientSSL- Running as Client IPV6" << std::endl;
  try {
    // We are using a TCP socket (SOCK_STREAM) over IPv6 with SSL
    const char *host = "os.ecci.ucr.ac.cr";
    const char *service = "https";
    Socket clientSocket('s', true, true);
    // Connect to the server using SSL at the given host and service
    clientSocket.SSLConnect(host, service);
    // Create and send an HTTP GET request
    std::ostringstream httpRequest;
    httpRequest << "GET /lego/list.php?figure=giraffe HTTP/1.1\r\n";
    httpRequest << "Host: " << host << "\r\n";
    httpRequest << "Connection: close\r\n";
    httpRequest << "\r\n";
    clientSocket.SSLWrite(httpRequest.str().c_str(), httpRequest.str().length());
    // Receive the HTTP response from the server
    char buffer[1024];
    int bytesRead;
    while ((bytesRead = clientSocket.SSLRead(buffer, sizeof(buffer) - 1)) > 0) {
      buffer[bytesRead] = '\0';
      std::cout << buffer;
    }
    // Close the connection
    clientSocket.Close();
  }
  catch (const SocketException &e) {
    std::cerr << "SSL Client IPv6 error: " << e.what() << std::endl;
  }
}
// Function to run the udp client-side code.
void runUdpClient() {
  std::cout << "-ClientUDP- Running as Client" << std::endl;
  int port = 8080;
  // using loopback address for testing
  const char* host = "127.0.0.1";
  const char* message = "Hello, server!";
  try {
    Socket client('d'); // Create a UDP socket
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, host, &serverAddr.sin_addr);
    client.sendTo(message, strlen(message) + 1, &serverAddr);
    std::cout << "-ClientUDP- Message sent: " << message << std::endl;
  } catch (const SocketException& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}
/// Function to run the server-side UDP code.
void runUdpServer() {
  int port = 8080;
  std::cout << "-ServerUDP- Running as Server" << std::endl;
  try {
    Socket server('d'); // Create a UDP socket
    server.Bind(port);
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    char buffer[1024];
    int bytesRead = server.recvFrom(buffer, clientAddrLen, &clientAddr);
    buffer[bytesRead] = '\0';
    std::cout << "-ServerUDP- Message received: " << buffer << std::endl;
  } catch (const SocketException& e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }
}

void runServerForShutdown() {
  try {
    Socket server('s'); // Create a TCP socket
    server.Bind(5000);  // Bind it to port 5000
    server.Listen(5);   // Listen for up to 5 connections
    int clientCount = 0;
    while (clientCount < 3) {
      Socket *client = server.Accept(); // Accept incoming connections
      // Receive messages from the client
      char buffer[1024];
      int bytesRead;
      while ((bytesRead = client->Read(buffer, sizeof(buffer))) > 0) {
        buffer[bytesRead] = '\0';
        std::cout << "Received message from client: " << buffer << std::endl;
      }
      // Close the connection and delete the client object
      client->Close();
      delete client;
      clientCount++;
    }
  } catch (const SocketException &e) {
    std::cerr << "Server error: " << e.what() << std::endl;
  }
}


void runClientForShutdown(int shutdownType) {
  try {
    Socket client('s'); // Create a TCP socket
    client.Connect("127.0.0.1", 5000); // Connect to the server at 127.0.0.1 and port 5000
    // Send a message to the server
    const char *message = "Hello, Server!";
    client.Write(message, strlen(message));
    // Shutdown the client's write end, signaling the server that no more data will be sent
    client.Shutdown(shutdownType);
    // Give the server some time to process the message before closing the client
    sleep(1);
    client.Close();

  } catch (const SocketException &e) {
    std::cerr << "Client error: " << e.what() << std::endl;
  }
}


