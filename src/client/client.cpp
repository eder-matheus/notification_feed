#include "client.h"
#include "common.h"
#include "notification.h"
#include "ui.h"
#include <iostream>
#include <pthread.h>
#include <string>
#include <time.h>

Client::Client(std::string username)
  : username_(username)
{
}

CmdType Client::validateCommand(std::string input, std::string& content) {

  std::string command = input.substr(0, input.find(' '));
  input.erase(0, input.find(' ') + sizeof(char));
  content = input;

  CmdType type;
  if (command == "FOLLOW") {
    std::cout << "follow command\n";
    type = CmdType::Follow;
  } else if (command == "SEND") {
    std::cout << "send command\n";
    type = CmdType::Send;
  } else {
    std::cout << "invalid command\n";
    type = CmdType::Error;
  }

  return type;
}

void *Client::commandToServer(void *args) {

  std::cout << "abri a thread de enviar comandos\n";
  std::string input;
  Client *_this = (Client *)args;
  Ui ui(FileType::None);
  ui.textBlock(UiType::Message, "teste");

  while (true) {
    std::getline(std::cin, input);
    std::string content;
    CmdType type = _this->validateCommand(input, content);

    if (type == CmdType::Error) {
      std::cout << "TO KILL LOOP\n";
      return 0;
    } else if (type == CmdType::Send) {
      Notification notification(content, _this->username_);
      notification.print();
      // send notification to server
    } else if (type == CmdType::Follow) {
      Follow follow(_this->username_, content);
      // send follow to server
    }
    std::cout << "end of loop\n";
  }
}

void *Client::receiveFromServer(void *args) {
  std::cout << "abri a thread de receber comandos\n";
  srand(time(NULL));
  int i = rand() % 1000000000000000 + 1;
  while (true) {
    Notification notification; // receive data from server through the UDP sockets
    if (i < 3)
      std::cout << "another user message is here!\n";
    i = rand() % 100000000000000 + 1;
  }
  return 0;
}

void Client::createConnection() {
  pthread_t senderTid;
  pthread_t dummyTid;

  pthread_create(&senderTid, NULL, commandToServer, (void *)this);
  pthread_create(&dummyTid, NULL, receiveFromServer, (void *)this);
  pthread_join(senderTid, NULL);
}
