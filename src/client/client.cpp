#include "client.h"
#include "common.h"
#include "notification.h"
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
    : username_(username), ready_to_receive_(false), ui(FileType::None) {}

CmdType Client::validateCommand(std::string input, std::string &content) {

  std::string command = input.substr(0, input.find(' '));
  input.erase(0, input.find(' ') + sizeof(char));
  content = input;

  CmdType type;
  if (command == "FOLLOW") {
    type = CmdType::Follow;
  } else if (command == "SEND") {
    type = CmdType::Send;
  } else if (command == "LOGOFF") {
    type = CmdType::Logoff;
  } else {
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
  server_answer = _this->tryCommand(packet, time_limit, true);

  if (server_answer == CMD_FAIL) {
    _this->ui.print(UiType::Error, "You reached the max simultaneous sessions.");
    exit(0);
  } else if (server_answer == CMD_404) {
    _this->ui.print(UiType::Error, "Server off, try again later.");
    exit(0);
  }

  _this->ready_to_receive_ = true;

  signal(SIGINT, sigintHandler);
  while (true) {

    std::getline(std::cin, input);

    if (std::cin.eof() || _interruption_) {
      input = "LOGOFF " + _this->username_;
    }
    std::string content;
    type = _this->validateCommand(input, content);

    if (type == CmdType::Error) {
      _this->ui.print(UiType::Error, "Invalid command.");
    } else {
      // send packet to server
      unsigned long int timestamp =
          duration_cast<milliseconds>(system_clock::now().time_since_epoch())
              .count();

      if (type == CmdType::Send && content.size() > 128) {
        _this->ui.print(UiType::Error, "Message has more than 128 characters.");
      } else {
        memset(packet, 0, BUFFER_SIZE);
        codificatePackage(packet, type, content, timestamp, _this->username_);
        server_answer = _this->tryCommand(packet, time_limit, false);

        if (type == CmdType::Logoff) {
          exit(0);
        }
      }
    }
  }
}

void *Client::receiveFromServer(void *args) {
  Client *_this = (Client *)args;
  Ui ui(FileType::None);

  _this->ui.print(UiType::Info, "Waiting for notifications.");
  char notification_packet[BUFFER_SIZE];
  unsigned int length = sizeof(struct sockaddr_in);
  std::vector<std::string> received_packet_data;
  while (true) {
    if (_this->ready_to_receive_) {
      memset(notification_packet, 0, BUFFER_SIZE);
      int n = recvfrom(_this->socket_, notification_packet, BUFFER_SIZE, 0,
                       (struct sockaddr *) &_this->from_, &length);
      if (n >= 0) {
        received_packet_data = decodificatePackage(notification_packet);
        std::string message = received_packet_data[1];
        unsigned long int timestamp = std::stoul(received_packet_data[2], nullptr, 10);
        std::string username = received_packet_data[3];
        ui.print(UiType::Message, message, username, timestamp);
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
    ui.print(UiType::Error, "Host does not exist.");
  } else {
    if ((socket_ = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
      ui.print(UiType::Error, "Failed to create socket.");
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
    ui.print(UiType::Error, "Failed to receive confirmation from server.");
  }
  
  received_packet_data = decodificatePackage(confirmation_packet);

  return received_packet_data[0];
}

std::string Client::tryCommand(char *packet, int time_limit, bool check_answer) {

  int secs_waiting_answer = 0;
  int n = -1;
  std::string server_answer = CMD_404;

  while(secs_waiting_answer < time_limit && server_answer == CMD_404 && n < 0) {
  
    n = sendto(socket_, packet, strlen(packet), 0, (const struct sockaddr *) &server_address_, 
     	       sizeof(struct sockaddr_in));
    if (n < 0) {
      ui.print(UiType::Error, "Failed to send command.");
    }
    else if (check_answer) {
      server_answer = checkServerAnswer();
      server_answer.append("\n"); //THIS NEEDS TO BE DEALT WITH
      secs_waiting_answer++;
    }
  }

  return server_answer;
}
