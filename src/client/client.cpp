#include "client.h"
#include "common.h"
#include "notification.h"
#include "ui.h"
#include <iostream>
#include <pthread.h>
#include <string>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring>
#include <strings.h>

Client::Client(std::string username)
  : username_(username),
    ready_to_receive_(false)
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
  char package[BUFFER_SIZE];
  CmdType login = CmdType::Login;

  codificatePackage(package, login, _this->username_);

  int n = sendto(_this->socket_, package, strlen(package), 0, (const struct sockaddr *) &_this->server_address_, sizeof(struct sockaddr_in));
  if (n < 0)
    std::cout << "ERRORR\n";

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
  Client *_this = (Client *)args;
  int i = 4;
  while (true) {
    if (_this->ready_to_receive_) {
      Notification notification; // receive data from server through the UDP sockets
      if (i < 3)
        std::cout << "another user message is here!\n";
      //i = rand() % 100000000000000 + 1;
    }
  }
  return 0;
}

void Client::createConnection(char* server, char* gate) {
  pthread_t senderTid;
  pthread_t dummyTid;
  
  server_ = gethostbyname(server);
  if(server == NULL) {
    std::cout << "ERROR, host does not exist\n";
  } else { 
    if ((socket_ = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
      std::cout << "ERROUU\n";
    server_address_.sin_family = AF_INET;
    server_address_.sin_port = htons(PORT);
    server_address_.sin_addr = *((struct in_addr *)server_->h_addr);
    bzero(&(server_address_.sin_zero), 8);

    pthread_create(&senderTid, NULL, commandToServer, (void *)this);
    pthread_create(&dummyTid, NULL, receiveFromServer, (void *)this);
    pthread_join(senderTid, NULL);
  }
}
