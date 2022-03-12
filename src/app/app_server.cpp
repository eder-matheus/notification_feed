#include "server.h"
#include <iostream>

int main() {
  std::cout << "Server started\n";

  Server server;
  server.createConnection();

  return 0;
}
