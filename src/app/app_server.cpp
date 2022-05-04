#include "server.h"
#include <iostream>

int main(int argc, char *argv[]) {
  Ui ui(FileType::None);

  if (argc < 2) {
    ui.print(UiType::Error, "Missing server ID.");
    return -1;
  }

  std::string server_id_str(argv[1]);
  int server_id = std::stoi(server_id_str);

  Server server;
  server.createConnection(server_id);

  return 0;
}
