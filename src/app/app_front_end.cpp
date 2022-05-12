#include "front_end.h"
#include <iostream>

int main(int argc, char *argv[]) {
  Ui ui(FileType::None);

  if (argc < 2) {
    ui.print(UiType::Error, "Missing port for front end.");
    return -1;
  }

  std::string front_end_port(argv[1]);

  FrontEnd front_end;
  front_end.createConnection(front_end_port);

  return 0;
}
