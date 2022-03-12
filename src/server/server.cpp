#include "server.h"
#include <iostream>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

Server::Server() {}

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

bool Server::notificationToUser(std::string user, int notification_id) {
  // send to client
  int n;
  char buf[BUFFER_SIZE];
  struct sockaddr_in client_address; // get client address from map
  n = sendto(socket_, buf, BUFFER_SIZE, 0, (struct sockaddr *)&client_address,
             sizeof(struct sockaddr));
  if (n < 0)
    printf("[ERROR] Cannot send to client.");

  // receive visualization confirmation

  notifications_[notification_id].decrementPendingReceivers();

  return true;
  // else return false if could not send
}

void *Server::receiveCommand(void *args) {
  Server *_this = (Server *)args;
  int n;
  char buf[BUFFER_SIZE];
  while (1) {
    // receive from client
    n = recvfrom(_this->socket_, buf, BUFFER_SIZE, 0,
                 (struct sockaddr *)&(_this->client_address_),
                 &(_this->client_length_));
    if (n < 0)
      printf("[ERROR] Cannot receive from client.");
    printf("Received a datagram: %s\n", buf);

    // send to cliente
    n = sendto(_this->socket_, "Got your message\n", BUFFER_SIZE, 0,
               (struct sockaddr *)&(_this->client_address_),
               sizeof(struct sockaddr));
    if (n < 0)
      printf("[ERROR] Cannot send to client.");

    CmdType received_command;
    // receive command from client

    if (received_command == CmdType::Send) {
      Notification received_notification;
      // receive notification from client
      // addNotification(received_notification);
      // change db
    } else if (received_command == CmdType::Follow) {
      Follow follow("ed", "er");
      // receive follow from client
      // followUser(follow);
      // change db
    }
  }
  return 0;
}

void *Server::sendNotifications(void *args) {
  Server *_this = (Server *)args;
  while (true) {
    for (auto &notification : _this->pending_notifications_) {
      auto &user = notification.first;
      auto &notification_ids = notification.second;
      for (int i = 0; notification_ids.size(); i++) {
        if (!_this->notificationToUser(user, notification_ids[i]))
          std::cout << "MESSAGE COULD NOT BE SENT TO USER\n" << std::endl;
      }
    }
  }
}

bool Server::logoffUser(std::string username) {
  if (users_.find(username) != users_.end()) { // user already exists on the db
    User &user = users_[username];
    ;
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

  client_length_ = sizeof(struct sockaddr_in);

  pthread_t senderTid;
  pthread_t receiverTid;

  pthread_create(&receiverTid, NULL, receiveCommand, (void *)this);
  pthread_create(&senderTid, NULL, sendNotifications, (void *)this);
}
