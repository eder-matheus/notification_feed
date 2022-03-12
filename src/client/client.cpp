#include "client.h"
#include "common.h"
#include "notification.h"
#include "ui.h"
#include <arpa/inet.h>
#include <chrono>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <string>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>

using namespace std::chrono;

// global var to check ctrl+c
bool _interruption_ = false;

void Client::sigintHandler(int sig_num)
{
  signal(SIGINT, sigintHandler);
  _interruption_ = true;
  std::cout << "Press ENTER to complete logoff\n";
}

Client::Client(std::string username)
    : username_(username), ready_to_receive_(false) {}

CmdType Client::validateCommand(std::string input, std::string &content) {

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
  } else if (command == "LOGOFF") {
    std::cout << "logoff\n";
    type = CmdType::Logoff;
  } else {
    std::cout << "invalid command\n";
    type = CmdType::Error;
  }

  return type;
}

void *Client::commandToServer(void *args) {

  std::string input;
  Client *_this = (Client *)args;
  char packet[BUFFER_SIZE], confirmation_packet[BUFFER_SIZE];
  CmdType type = CmdType::Login;
  unsigned int length = 0;  

  codificatePackage(packet, type, _this->username_);

  int n = sendto(_this->socket_, packet, strlen(packet), 0,
                 (const struct sockaddr *)&_this->server_address_,
                 sizeof(struct sockaddr_in));
  if (n < 0)
    std::cout << "\nfailed to send login\n";
  
  length = sizeof(struct sockaddr_in);
 
  n = recvfrom(_this->socket_, confirmation_packet, strlen(confirmation_packet), 0,
	       (struct sockaddr *) &_this->from_, &length);
  if (n < 0)
    std::cout << "\nfailed to receive\n";
  
  std::cout << confirmation_packet;

  _this->ready_to_receive_ = true;

  Ui ui(FileType::None);
  ui.textBlock(UiType::Message, "teste");

  signal(SIGINT, sigintHandler);
  while (true) {
    std::getline(std::cin, input);
    if (std::cin.eof() || _interruption_) {
      input = "LOGOFF " + _this->username_;
    }
    std::string content;
    type = _this->validateCommand(input, content);

    if (type == CmdType::Error) {
      std::cout << "TO KILL LOOP\n";
      return 0;
    } else {
      // send packet to server
      unsigned long int timestamp =
          duration_cast<milliseconds>(system_clock::now().time_since_epoch())
              .count();
      codificatePackage(packet, type, content, timestamp, _this->username_);
      n = sendto(_this->socket_, packet, strlen(packet), 0,
                 (const struct sockaddr *)&_this->server_address_,
                 sizeof(struct sockaddr_in));
      if (n < 0)
        std::cout << "\nfailed to send cmd\n";

      // need to add a check for the return of the server
      if (type == CmdType::Logoff) {
        exit(0);
      }
    }
    std::cout << "end of loop\n";
  }
}

void *Client::receiveFromServer(void *args) {
  Client *_this = (Client *)args;
  int i = 4;
  std::cout << "receiving\n";
  while (true) {
    if (_this->ready_to_receive_) {
      Notification
          notification; // receive data from server through the UDP sockets
      if (i < 3)
        std::cout << "another user message is here!\n";
      // i = rand() % 100000000000000 + 1;
    }
  }
  return 0;
}

void Client::createConnection(char *server, std::string gate) {
  pthread_t senderTid;
  pthread_t receiverTid;

  server_ = gethostbyname(server);
  if (server == NULL) {
    std::cout << "\nhost does not exist\n";
  } else {
    if ((socket_ = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
      std::cout << "\nfailed to create socket\n";
    server_address_.sin_family = AF_INET;
    server_address_.sin_port = htons(std::stoi(gate));
    server_address_.sin_addr = *((struct in_addr *)server_->h_addr);
    bzero(&(server_address_.sin_zero), 8);

    pthread_create(&senderTid, NULL, commandToServer, (void *)this);
    pthread_create(&receiverTid, NULL, receiveFromServer, (void *)this);
    pthread_join(senderTid, NULL);
  }
}
