#include "client.h"
#include "common.h"
#include <iostream>

bool validateUsername(char *name) {
  Ui ui(FileType::None);

  std::string username(name);

  int num_of_chars = username.length();
  if (num_of_chars > 20 || num_of_chars < 4) {
    ui.print(UiType::Error, "Invalid name size (min 4; max 20).");
    return false;
  }
  if (username[0] != '@') {
    ui.print(UiType::Error, "User name should start with '@'.");
    return false;
  }

  return true;
}

int main(int argc, char *argv[]) {
  Ui intro(FileType::Intro);
  if (argc < 4) {
    intro.print(UiType::Error, "Missing username, server or gate.");
    return -1;
  }

  if (!validateUsername(argv[1])) {
    return -2;
  }

  std::string name = argv[1];
  char *server = argv[2];
  std::string gate = argv[3];

  Client client(name);

  intro.asciiArt();

  client.createConnection(server, gate);

  Ui exit(FileType::Exit);
  exit.asciiArt();

  return 0;
}
