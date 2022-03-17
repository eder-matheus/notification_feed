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
  char packet[BUFFER_SIZE];
  CmdType type;
  std::string server_answer = CMD_404;  
  int n = 0, time_limit = REC_WAIT_LIMIT;

  memset(packet, 0, BUFFER_SIZE);
  codificatePackage(packet, CmdType::Login, _this->username_);
  server_answer = _this->tryCommand(packet, time_limit);

  if (server_answer == CMD_FAIL) {
    std::cout << "already logged\n";
    exit(0);
  } else if (server_answer == CMD_404) {
    std::cout << "server off, try again later\n";
    exit(0);
  }

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

      memset(packet, 0, BUFFER_SIZE);
      codificatePackage(packet, type, content, timestamp, _this->username_);
      server_answer = _this->tryCommand(packet, time_limit);    
      
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
  Ui ui(FileType::None);

  std::cout << "receiving\n";
  char notification_packet[BUFFER_SIZE];
  unsigned int length = sizeof(struct sockaddr_in);
  std::vector<std::string> received_packet_data;
  while (true) {
    if (_this->ready_to_receive_) {
      memset(notification_packet, 0, BUFFER_SIZE);
      int n = recvfrom(_this->socket_, notification_packet, BUFFER_SIZE, 0,
                       (struct sockaddr *) &_this->from_, &length);
      if (n < 0) {
        std::cout << "\n failed to receive \n";
      } else {
        std::cout << "received from server: " << notification_packet << "\n";

        received_packet_data = decodificatePackage(notification_packet);
        std::string message = received_packet_data[1];
        unsigned long int timestamp = std::stoul(received_packet_data[2], nullptr, 10);
        std::string username = received_packet_data[3];
        ui.textBlock(UiType::Message, message, username, timestamp);
      }
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
    socket_time_.tv_sec = REC_WAIT;
    socket_time_.tv_usec = 0;
    setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, &socket_time_, sizeof(struct timeval));
    server_address_.sin_family = AF_INET;
    server_address_.sin_port = htons(std::stoi(gate));
    server_address_.sin_addr = *((struct in_addr *)server_->h_addr);
    bzero(&(server_address_.sin_zero), 8);

    pthread_create(&senderTid, NULL, commandToServer, (void *)this);
    pthread_create(&receiverTid, NULL, receiveFromServer, (void *)this);
    pthread_join(senderTid, NULL);
  }
}

std::string Client::checkServerAnswer() {
 
  char confirmation_packet[BUFFER_SIZE];
  unsigned int length = sizeof(struct sockaddr_in);
  std::vector<std::string> received_packet_data;
  int n;

  memset(confirmation_packet, 0, BUFFER_SIZE);
  strcpy(confirmation_packet, CMD_404);

  n = recvfrom(socket_, confirmation_packet, BUFFER_SIZE, 0,
                 (struct sockaddr *) &from_, &length);

  if (n < 0) {
    std::cout << "\nfailed to receive confirmation from server! \n";
  }
  
  received_packet_data = decodificatePackage(confirmation_packet);

  return received_packet_data[0];
}

std::string Client::tryCommand(char *packet, int time_limit) {

  int secs_waiting_answer = 0, n;
  std::string server_answer = CMD_404;

  while(secs_waiting_answer < time_limit && server_answer == CMD_404) {
  
    n = sendto(socket_, packet, strlen(packet), 0, (const struct sockaddr *) &server_address_, 
     	       sizeof(struct sockaddr_in));
    if (n < 0) {
      std::cout << "failed to send cmd\n";
    }
    else {
      server_answer = checkServerAnswer();
      server_answer.append("\n"); //THIS NEEDS TO BE DEALT WITH
      secs_waiting_answer++;
      std::cout << "waited seconds: " << secs_waiting_answer << "\n";
    }
  }

  return server_answer;
}
