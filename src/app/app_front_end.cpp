#include "front_end.h"
#include <iostream>

int main(int argc, char *argv[]) {
  Ui ui(FileType::None);

  if (argc < 3) {
    ui.print(UiType::Error, "Missing server or port for front end.");
    return -1;
  }

  std::string server(argv[1]);
  std::string front_end_port(argv[2]);

  FrontEnd front_end;
  front_end.createConnection(server, front_end_port);

  return 0;
}
