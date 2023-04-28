#include "test.hpp"
int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << 
      " <1:server|2:client|3:ssl_client_Ipv4|4:ssl_client_Ipv6>" << std::endl;
    return 1;
  }
  int mode = std::atoi(argv[1]);
  switch (mode) {
    case 1:
      runServer();
      break;
    case 2:
      runClient();
      break;
    case 3:
      runSslClientIpv4();
      break;
    case 4:
      runSslClientIpv6();
      break;
    default:
      std::cerr << 
      "<1:server|2:client|3:ssl_client_Ipv4|4:ssl_client_Ipv6>" << 
      std::endl;
      return 1;
  }

  return 0;
}


