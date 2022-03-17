#include "server.h"
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <cstring>

Server::Server() {}

bool Server::isLogged(const std::string &username) {
  return logged_users_.find(username) != logged_users_.end();
}

bool Server::loginUser(std::string username, struct sockaddr_in user_address) {
  if (users_.find(username) != users_.end()) { // user already exists on the db
    User &user = users_[username];
    if (user.getSessions() >= MAX_SESSIONS) {
      return false;
    }
    user.incrementSessions();
  } else { // creating new user
    users_[username] = User(username);
    users_[username].incrementSessions();
  }

  logged_users_[username].push_back(user_address);

  return true;
}

bool Server::logoffUser(std::string username) {
  if (users_.find(username) != users_.end()) { // user already exists on the db
    User &user = users_[username];
    user.decrementSessions();

    // if user has no sessions on, delete the list of addresses of it
    if (user.getSessions() == 0) {
      logged_users_.erase(username);
    }

    return true;
  }

  return false;
}

bool Server::followUser(Follow follow) {
  std::string curr_user = follow.client;
  std::string user_followed = follow.user_followed;

  if (users_.find(user_followed) != users_.end()) { // user exists on the db
    User &user = users_[user_followed];
    user.addFollower(curr_user);
    return true;
  }

  // return false
  return false;
}

void Server::addNotification(const Notification &notification) {
  // add new notification to the notifications map
  notifications_[new_notification_id_] = notification;
  notifications_[new_notification_id_].setId(new_notification_id_);

  // add the notification to the list of pending notifications
  // for the followers of the user who created the notification
  const std::vector<std::string> &followers =
      users_[notification.getUsername()].getFollowers();
  for (const std::string &follower : followers) {
    pending_notifications_[follower].push_back(new_notification_id_);
  }
  notifications_[new_notification_id_].setPendingReceivers(followers.size());

  // increment new_notification_id_ to have a valid id for the next added
  // notification
  new_notification_id_++;
}

bool Server::notificationToUser(const std::string &user, int notification_id) {
  const Notification &notification = notifications_[notification_id];

  // send to client
  int n;
  char packet[BUFFER_SIZE]; // convert notification to string
  codificatePackage(packet, CmdType::Send, notification.getMessage(),
                    notification.getTimestamp(), notification.getUsername());
  std::vector<struct sockaddr_in>
      client_addresses = logged_users_[user]; // get client addresses from map
  for (struct sockaddr_in &addr : client_addresses) {
    n = sendto(socket_, packet, BUFFER_SIZE, 0, (struct sockaddr *)&addr,
               sizeof(struct sockaddr));
    if (n < 0) {
      printf("[ERROR] Cannot send to client.");
      return false;
    }
    // receive visualization confirmation
  }

  notifications_[notification_id].decrementPendingReceivers();

  // delete notification from server if there is no pending receivers
  if (notifications_[notification_id].getPendingReceivers() == 0) {
    notifications_.erase(notification_id);
  }

  return true;
}

void *Server::receiveCommand(void *args) {
  std::cout << "Read to receive commands\n";
  Server *_this = (Server *)args;
  struct sockaddr_in client_address;
  socklen_t client_length = sizeof(struct sockaddr_in);
  int n;
  char packet[BUFFER_SIZE], confirmation_packet[BUFFER_SIZE];

  while (1) {
    // receive from client
    memset(packet, 0, BUFFER_SIZE);
    n = recvfrom(_this->socket_, packet, BUFFER_SIZE, 0,
                 (struct sockaddr *)&(client_address), &(client_length));
    if (n < 0)
      printf("[ERROR] Cannot receive command from client.");
    printf("Received a datagram: %s\n", packet);

    std::vector<std::string> decoded_packet = decodificatePackage(packet);
    std::string received_command = decoded_packet[0];

    if (received_command == "send") {
      std::string message = decoded_packet[1];
      unsigned long int timestamp = std::stoul(decoded_packet[2], nullptr, 10);
      std::string username = decoded_packet[3];

      Notification received_notification(message, timestamp, username);
      _this->addNotification(received_notification);
      std::cout << "Received notification: ";
      received_notification.print();

      // send confirmation to client
      if (_this->sendCmdStatus(CMD_OK, confirmation_packet, client_address) < 0) {
        printf("[ERROR] Cannot send notification confirmation to client.\n");
      }
      // update db
    } else if (received_command == "follow") {
      std::string followed_user = decoded_packet[1];
      std::string username = decoded_packet[2];
      Follow follow(username, followed_user);
      bool follow_ok = _this->followUser(follow);
      if (follow_ok) {
        std::cout << username << " followed " << followed_user << "\n";
        // send confirmation to client
        if (_this->sendCmdStatus(CMD_OK, confirmation_packet, client_address) < 0) {
          printf("[ERROR] Cannot send follow confirmation to client.\n");
        }
      } else {
        std::cout << "[ERROR]" << followed_user << " not found.\n";
        // send confirmation to client
        if (_this->sendCmdStatus(CMD_FAIL, confirmation_packet, client_address) < 0) {
          printf("[ERROR] Cannot send follow confirmation to client.\n");
        }
      }
      // change db
    } else if (received_command == "login") {
      std::string username = decoded_packet[1];
      bool login_ok = _this->loginUser(username, client_address);
      if (login_ok) {
        std::cout << username << " successfully logged.\n";
        // send confirmation to client
        if (_this->sendCmdStatus(CMD_OK, confirmation_packet, client_address) < 0) {
          printf("[ERROR] Cannot send login confirmation to client.\n");
        }
      } else {
        std::cout << "[ERROR]" << username << " has reached max sessions\n";
        // send confirmation to client
        if (_this->sendCmdStatus(CMD_FAIL, confirmation_packet, client_address) < 0) {
          printf("[ERROR] Cannot send login confirmation to client.\n");
        }
      }
    } else if (received_command == "logoff") {
      std::string username = decoded_packet[1];
      _this->logoffUser(username);
      // send confirmation to client
      if (_this->sendCmdStatus(CMD_OK, confirmation_packet, client_address) < 0) {
        printf("[ERROR] Cannot send logoff confirmation to client.\n");
      }
    } else {
      std::cout << "[ERROR] Command not identified\nDecoded packet:\n";
      for (std::string word : decoded_packet) {
        std::cout << "\t" << word << "\n";
      }
    }
    std::cout << "end loop\n";
  }
  return 0;
}

void *Server::sendNotifications(void *args) {
  Server *_this = (Server *)args;
  std::cout << "Read to send notifications\n";
  std::vector<std::string> logged_users;
  while (true) {
    for (auto &notification : _this->pending_notifications_) {
      const auto &user = notification.first;
      if (_this->isLogged(user)) {
        logged_users.push_back(user);
        auto &notification_ids = notification.second;
        for (int i = 0; i < notification_ids.size(); i++) {
          if (!_this->notificationToUser(user, notification_ids[i])) {
            std::cout << "[ERROR] Notification not sent\n";
          }
          else {
            std::cout << "Notification sent\n";
          }
        }
      }
    }
    for (std::string user : logged_users) {
      _this->pending_notifications_.erase(user);
    }
  }
}

void Server::createConnection() {
  if ((socket_ = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    printf("[ERROR] Cannot open socket.");
    exit(1);
  }

  server_address_.sin_family = AF_INET;
  server_address_.sin_port = htons(PORT);
  server_address_.sin_addr.s_addr = INADDR_ANY;
  bzero(&(server_address_.sin_zero), 8);

  if (bind(socket_, (struct sockaddr *)&server_address_,
           sizeof(struct sockaddr)) < 0)
    printf("[ERROR] Cannot perform binding.");

  socklen_t client_length = sizeof(struct sockaddr_in);

  pthread_t senderTid;
  pthread_t receiverTid;

  pthread_create(&receiverTid, NULL, receiveCommand, (void *)this);
  pthread_create(&senderTid, NULL, sendNotifications, (void *)this);

  pthread_join(receiverTid, NULL);
  pthread_join(senderTid, NULL);
}

int Server::sendCmdStatus(std::string status, char* confirmation_packet, struct sockaddr_in client_address) {
  memset(confirmation_packet, 0, BUFFER_SIZE);
  codificatePackage(confirmation_packet, CmdType::Confirmation, status);
  int n = sendto(socket_, confirmation_packet, strlen(confirmation_packet), 0,
             (struct sockaddr *)&(client_address), sizeof(struct sockaddr_in));

  return n;
}
