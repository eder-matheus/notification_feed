#include "client.h"
#include "common.h"
#include "ui.h"
#include <iostream>

bool validateUsername(char *name) {

  std::string username(name);

  int num_of_chars = username.length();
  if (num_of_chars > 20 || num_of_chars < 4) {
    std::cout << "invalid name size (min 4; max 20)\n";
    return false;
  }
  if (username[0] != '@') {
    std::cout << "User name should start with '@'\n";
    return false;
  }

  return true;
}

int main(int argc, char *argv[]) {

  if (argc < 4) {
    std::cout << "missing username, server or gate\n";
    return -1;
  }

  if (!validateUsername(argv[1])) {
    return -2;
  }

  std::string name = argv[1];
  char *server = argv[2];
  char *gate = argv[3];

  Ui intro(FileType::Intro);
  Client client(name);

  intro.asciiArt();

  client.createConnection(server, gate);

  return 0;
}
