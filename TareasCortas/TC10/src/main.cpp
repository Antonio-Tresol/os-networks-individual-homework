/**
 *   UCR-ECCI
 *   CI-0123 Proyecto integrador de redes y sistemas operativos
 *
 *   Socket client/server example with threads
 *
 **/
#include <cstdio>   // printf
#include <cstdlib>  // atoi
#include <cstring>  // strlen, strcmp
#include <thread>

#include "Socket.hpp"

#define PORT 8080

void Service(Socket* client) {
  char buf[1024] = {0};
  int bytes;
  const char* ServerResponse =
      "\n<Body>\n\
\t<Server>os.ecci.ucr.ac.cr</Server>\n\
\t<dir>ci0123</dir>\n\
\t<Name>Proyecto Integrador Redes y sistemas Operativos</Name>\n\
\t<NickName>PIRO</NickName>\n\
\t<Description>Consolidar e integrar los conocimientos de redes y sistemas operativos</Description>\n\
\t<Author>profesores PIRO</Author>\n\
</Body>\n";
  const char* validMessage =
      "\n<Body>\n\
\t<UserName>piro</UserName>\n\
\t<Password>ci0123</Password>\n\
</Body>\n";
  try {
    client->SSLAccept();
    client->SSLShowCerts();
    bytes = client->SSLRead(buf, sizeof(buf));
    buf[bytes] = '\0';
    printf("Client msg: \"%s\"\n", buf);

    if (!strcmp(validMessage, buf)) {
      client->SSLWrite(ServerResponse, strlen(ServerResponse));
    } else {
      client->SSLWrite("Invalid Message", strlen("Invalid Message"));
    }
    client->Close();
    delete client;
  } catch (const std::exception& e) {
    std::cerr << "Server error: " << e.what() << std::endl;
  }
}

int main(int cuantos, char** argumentos) {
  if (cuantos != 2) {
    printf("Uso: %s <1|2>\n", argumentos[0]);
    printf("\t1: Server\n");
    printf("\t2: Client\n");
    return 1;
  }
  int mode = std::atoi(argumentos[1]);
  if (mode == 1) {
    Socket *server, *client;
    try {
      server =
          new Socket('s', PORT,
                     "/home/abotresol/Documents/Uni/OS/abadillaolivas_ci-0123/"
                     "TrabajoEnClase/TC10/certs/ci0123.pem",
                     "/home/abotresol/Documents/Uni/OS/abadillaolivas_ci-0123/"
                     "TrabajoEnClase/TC10/certs/ci0123.pem",
                     true);
      for (int i = 0; i < 2; i++) {
        std::cout << "Waiting for connection..." << std::endl;
        client = server->Accept();
        client->SSLCreate(server);
        std::thread worker(Service, client);
        worker.join();  // Join the worker thread to wait for it to finish
                        // before continuing the main thread.
      }                 // service connection
      delete server;
    } catch (const std::exception& e) {
      std::cerr << e.what() << '\n';
      return 1;
    }
  } else if (mode == 2) {
    Socket* client;
    char userName[16] = "piro";
    char password[16] = "ci0123";
    const char* requestMessage =
        "\n<Body>\n\
\t<UserName>%s</UserName>\n\
\t<Password>%s</Password>\n\
</Body>\n";
    char buf[1024];
    char clientRequest[1024] = {0};
    int bytes;
    std::string hostname = "0:0:0:0:0:0:0:1";  // loopback address for ipv6
    int portnum = PORT;
    try {
      client = new Socket('s', true, true);
      sprintf(clientRequest, requestMessage, userName,
              password);  // construct reply
      client->SSLConnect(hostname.c_str(), portnum);
      printf("\n\nConnected with %s encryption\n", client->SSLGetCipher());
      client->SSLShowCerts();  // display any certs
      client->SSLWrite(clientRequest,
                       strlen(clientRequest));    // encrypt & send message
      bytes = client->SSLRead(buf, sizeof(buf));  // get reply & decrypt
      buf[bytes] = 0;
      printf("Received: \"%s\"\n", buf);
      delete client;
    } catch (const std::exception& e) {
      std::cerr << e.what() << '\n';
      try {
        std::rethrow_if_nested(e);
      } catch (const std::exception& ne) {
        std::cerr << "Nested exception caught: " << ne.what() << std::endl;
      }
    }
  } else if (mode == 3) {
    Socket *server, *client;
    try {
      server =
          new Socket('s', PORT,
                     "/home/abotresol/Documents/Uni/OS/abadillaolivas_ci-0123/"
                     "TrabajoEnClase/TC10/certs/ci0123.pem",
                     "/home/abotresol/Documents/Uni/OS/abadillaolivas_ci-0123/"
                     "TrabajoEnClase/TC10/certs/ci0123.pem");
      for (int i = 0; i < 2; i++) {
        std::cout << "Waiting for connection..." << std::endl;
        client = server->Accept();
        std::cout << "Connection accepted" << std::endl;
        std::cout << "creating ssl for socket" << std::endl;
        client->SSLCreate(server);
        std::cout << "ssl created, serving client" << std::endl;

        // Use fork() instead of thread
        pid_t pid = fork();
        if (pid == 0) {  // Child process
          Service(client);
          exit(0);
        } else if (pid > 0) {  // Parent process
          client->Close();
        } else {
          perror("fork");
          exit(EXIT_FAILURE);
        }
      }
      delete server;
    } catch (const std::exception& e) {
      std::cerr << e.what() << '\n';
      return 1;
    }
  }
  return 0;
}