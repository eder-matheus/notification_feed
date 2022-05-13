#include "front_end.h"
#include "common.h"
#include <iostream>
#include <map>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>

void FrontEnd::sigintHandler(int sig_num) {
  signal(SIGINT, sigintHandler);
  std::cout << "Front end closed.\n";
  exit(0);
}

FrontEnd::FrontEnd() : ui_(FileType::None) {}

bool FrontEnd::readServers() {
  std::string line;
  std::ifstream cfg(cfg_file_name_);
  if (cfg.is_open()) {
    while (std::getline(cfg, line)) {
      if (line[0] == '#')
        continue;
      int id = std::stoi(line.substr(0, line.find(' ')));
      line.erase(0, line.find(' ') + sizeof(char));
      int port = std::stoi(line.substr(0, line.find(' ')));
      if (servers_ports_.find(id) == servers_ports_.end()) {
        servers_ports_[id] = port;
      }
    }
  } else {
    return false;
  }

  setPrimaryServer(1);

  return true;
}

void FrontEnd::setPrimaryServer(int id) { primary_server_id_ = id; }

void FrontEnd::createConnection(const std::string& server_name, const std::string& port) {
  struct hostent *server = gethostbyname(server_name.c_str());
  if (server == NULL) {
    ui_.print(UiType::Error, "Host does not exist.");
  } else {
    if ((socket_ = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
      ui_.print(UiType::Error, "Cannot open socket.");
      exit(1);
    }
    struct timeval socket_time;
    socket_time.tv_sec = REC_WAIT;
    socket_time.tv_usec = 0;
    setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, &socket_time,
               sizeof(struct timeval));

    if (!readServers()) {
      ui_.print(UiType::Warn, "File " + cfg_file_name_ + " not found.");
    }

    ui_.print(UiType::Success, "Front end started using server " +
                                   std::to_string(primary_server_id_) +
                                   " and port " + port + ".");

    front_end_address_.sin_family = AF_INET;
    front_end_address_.sin_port = htons(std::stoi(port));
    front_end_address_.sin_addr = *((struct in_addr *)server->h_addr);
    bzero(&(front_end_address_.sin_zero), 8);

    server_address_.sin_family = AF_INET;
    server_address_.sin_port = htons(servers_ports_[primary_server_id_]);
    server_address_.sin_addr = *((struct in_addr *)server->h_addr);
    bzero(&(server_address_.sin_zero), 8);

    if (bind(socket_, (struct sockaddr *)&front_end_address_,
             sizeof(struct sockaddr)) < 0)
      ui_.print(UiType::Error,
                "Cannot perform binding on front end " + port + ".");

    pthread_t receiverTid;
    pthread_create(&receiverTid, NULL, receive, (void *)this);
    pthread_join(receiverTid, NULL);
  }
}

void *FrontEnd::receive(void *args) {
  FrontEnd *_this = (FrontEnd *)args;

  char packet[BUFFER_SIZE];
  unsigned int length = sizeof(struct sockaddr_in);
  std::string command;
  struct sockaddr_in from;

  signal(SIGINT, sigintHandler);
  while (true) {
    memset(packet, 0, BUFFER_SIZE);
    int n = recvfrom(_this->socket_, packet, BUFFER_SIZE, 0,
                     (struct sockaddr *)&from, &length);
    if (n >= 0) {
      std::vector<std::string> decoded_package = decodificatePackage(packet);
      command = decoded_package[0];

      if (_this->fromServer(command)) {
        std::cout << "Received from server: " << packet << "\n";
        if (command == CMD_OK) {
          _this->server_address_ = from;
        }

        if (command == "set_leader") {
          _this->setPrimaryServer(std::stoi(decoded_package[1]));
          _this->server_address_.sin_port = htons(_this->servers_ports_[_this->primary_server_id_]);
        } else {
          sendto(_this->socket_, packet, strlen(packet), 0,
                 (const struct sockaddr *)&_this->client_address_,
                 sizeof(struct sockaddr_in));
        }
      } else {
        if (command == "login") {
          _this->client_address_ = from;
        }
        std::cout << "Received from client: " << packet << "\n";
        int n = sendto(_this->socket_, packet, strlen(packet), 0,
                       (const struct sockaddr *)&_this->server_address_,
                       sizeof(struct sockaddr_in));
        if (n < 0) {
          _this->ui_.print(UiType::Error,
                           "Failed to send package to port " +
                               std::to_string(_this->server_address_.sin_port) +
                               ".");
        } else {
          _this->ui_.print(UiType::Info,
                           "Package sent to port " +
                               std::to_string(_this->server_address_.sin_port) +
                               ".");
        }

        if (command == "logoff") {
          return 0;
        }
      }
    }
  }
  return 0;
}

bool FrontEnd::fromServer(const std::string &command) {
  bool from_server = false;

  if (command == "follow" || command == "login" || command == "logoff" ||
      command == "client_connect" || command == "send") {
    from_server = false;
  } else {
    from_server = true;
  }

  return from_server;
}
