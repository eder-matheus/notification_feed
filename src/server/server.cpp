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

Server::Server() {}

bool Server::isLogged(const std::string &username) {
  return logged_users_.find(username) != logged_users_.end();
}

bool Server::loginUser(std::string username) {
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

  return true;
}

bool Server::logoffUser(std::string username) {
  if (users_.find(username) != users_.end()) { // user already exists on the db
    User &user = users_[username];
    user.decrementSessions();

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
  char package[BUFFER_SIZE]; // convert notification to string
  codificatePackage(package, CmdType::Receive, notification.getMessage(),
                    notification.getTimestamp(), notification.getUsername());
  std::vector<struct sockaddr_in>
      client_addresses; // get client addresses from map
  for (struct sockaddr_in &addr : client_addresses) {
    n = sendto(socket_, package, BUFFER_SIZE, 0, (struct sockaddr *)&addr,
               sizeof(struct sockaddr));
    if (n < 0)
      printf("[ERROR] Cannot send to client.");
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
  socklen_t client_length;
  int n;
  char package[BUFFER_SIZE];
  while (1) {
    // receive from client
    n = recvfrom(_this->socket_, package, BUFFER_SIZE, 0,
                 (struct sockaddr *)&(client_address), &(client_length));
    if (n < 0)
      printf("[ERROR] Cannot receive from client.");
    printf("Received a datagram: %s\n", package);

    // send to cliente
    n = sendto(_this->socket_, "Got your package\n", BUFFER_SIZE, 0,
               (struct sockaddr *)&(client_address), sizeof(struct sockaddr));
    if (n < 0)
      printf("[ERROR] Cannot send to client.");

    std::vector<std::string> decoded_package = decodificatePackage(package);
    std::string received_command = decoded_package[0];
    // receive command from client

    if (received_command == "send") {
      std::string message = decoded_package[1];
      int timestamp = std::stoi(decoded_package[2]);
      std::string username = decoded_package[3];
      
      Notification received_notification(message, timestamp, username);
      _this->addNotification(received_notification);
      // update db
    } else if (received_command == "follow") {
      // receive follow from client
      Follow follow; // use decode to get Follow from the received package
      bool follow_ok = _this->followUser(follow);
      if (follow_ok) {
        n = sendto(_this->socket_, "Successfully followed.\n", 25, 0,
                   (struct sockaddr *)&(client_address),
                   sizeof(struct sockaddr));
      } else {
        n = sendto(_this->socket_, "Failed to follow.\n", 25, 0,
                   (struct sockaddr *)&(client_address),
                   sizeof(struct sockaddr));
      }
      // change db
    } else if (received_command == "login") {
      std::string username; // get username from the received package
      bool login_ok = _this->loginUser(username);
      if (login_ok) {
        n = sendto(_this->socket_, "Successfully logged.\n", 25, 0,
                   (struct sockaddr *)&(client_address),
                   sizeof(struct sockaddr));
      } else {
        n = sendto(_this->socket_, "Failed to log.\n", 25, 0,
                   (struct sockaddr *)&(client_address),
                   sizeof(struct sockaddr));
      }
    } else {                // CmdType::Logoff
      std::string username; // get username from the received package
      _this->logoffUser(username);
      _this->logged_users_[username].clear();
    }
  }
  return 0;
}

void *Server::sendNotifications(void *args) {
  Server *_this = (Server *)args;
  std::cout << "Read to send notifications\n";
  while (true) {
    for (auto &notification : _this->pending_notifications_) {
      const auto &user = notification.first;
      if (_this->isLogged(user)) {
        auto &notification_ids = notification.second;
        for (int i = 0; notification_ids.size(); i++) {
          if (!_this->notificationToUser(user, notification_ids[i]))
            std::cout << "MESSAGE COULD NOT BE SENT TO USER\n" << std::endl;
        }
      }
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
