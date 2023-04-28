// Copyright 2023 Antonio Badilla Olivas <anthonny.badilla@ucr.ac.cr>.
#include <thread>

#include "test.hpp"
/**
 * @file main.cpp
 * @brief main to run test for .
 */

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0]
              << " <1:server and client TCP|2:ssl client Ipv4|3:ssl client "
                 "Ipv6|4:Server and Client UDP|5:Shutdown>"
              << std::endl;
    return 1;
  }
  int mode = std::atoi(argv[1]);
  switch (mode) {
    case 1: {
      std::thread serverThread(runServer);
      std::thread clientThread(runClient);
      serverThread.join();
      clientThread.join();
      break;
    }
    case 2: {
      std::thread clientThread(runSslClientIpv4);
      clientThread.join();
      break;
    }
    case 3: {
      std::thread clientThread(runSslClientIpv6);
      clientThread.join();
      break;
    }
    case 4: {
      std::thread serverThread(runUdpServer);
      std::thread clientThread(runUdpClient);
      serverThread.join();
      clientThread.join();
      break;
    }
    case 5: {
      std::thread serverThread(runServerForShutdown);
      std::thread clientThread(runClientForShutdown, SHUT_WR);
      std::thread clientThread2(runClientForShutdown, SHUT_RDWR);
      serverThread.join();
      clientThread.join();
      clientThread2.join();
      break;
    }
    default: {
      std::cerr << "Invalid mode: " << mode << std::endl;
      break;
    }
      return 0;
  }
}
