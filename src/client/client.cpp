#include "client.h"
#include "definitions.h"
#include "ui.h"
#include <iostream>
#include <string>
#include <time.h>

Client::Client() {}

int Client::validateCommand(std::string input) {

  std::string command = input.substr(0, input.find(' '));
  input.erase(0, input.find(' ') + sizeof(char));
  std::string message = input;

  if (command.compare(CMD_FOLLOW) == 0) {
    std::cout << "follow command\n";
  } else if (command.compare(CMD_SEND) == 0) {
    std::cout << "send command\n";
  } else {
    std::cout << "invalid command\n";
    return -1;
  }

  return 0;
}

void *Client::commandToServer(void *args) {

  std::cout << "abri a thread de enviar comandos\n";
  std::string input;
  Client *_this = (Client *)args;
  Ui ui(NONE);
  ui.textBlock(MSG, "teste");

  while (true) {
    std::getline(std::cin, input);
    if (_this->validateCommand(input) < 0) {
      std::cout << "TO KILL LOOP\n";
      return 0;
    }
    std::cout << "end of loop\n";
  }
}

void *Client::dummy(void *args) {
  std::cout << "abri a thread de receber comandos\n";
  srand(time(NULL));
  int i = rand() % 10000000000 + 1;
  while (true) {
    if (i < 3)
      std::cout << "another user message is here!\n";
    i = rand() % 1000000000 + 1;
  }
  return 0;
}

void Client::createConnection() {
  pthread_t senderTid;
  pthread_t dummyTid;

  pthread_create(&senderTid, NULL, commandToServer, (void *)this);
  pthread_create(&dummyTid, NULL, dummy, (void *)this);
  pthread_join(senderTid, NULL);
}
