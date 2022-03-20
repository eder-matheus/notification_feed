#include "server.h"
#include <iostream>

int main() {
  Ui ui(FileType::None);
  ui.print(UiType::Success, "Server started.");

  Server server;
  server.createConnection();

  return 0;
}
