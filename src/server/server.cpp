#include "server.h"
#include <iostream>
#include <string>
#include <time.h>

Server::Server() {}

bool Server::loginUser(std::string username)
{
  if (users_.find(username) != users_.end()) { // user already exists on the db
    User& user = users_[username];
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

int Server::notificationToUser(std::string user, int notification_id) {
  //send notification
  notifications_[notification_id].decrementPendingReceivers();
  return 0;
  //else return -1 if could not send
}

void* Server::receiveCommand(void *args) {
  //receive enum of command (Follow or Send) from client
  //prepare to receive follow struct or notification object
  //change database and local structure
}

void* Server::sendNotifications(void *args) {
  while(true) {
    for(auto& [user, notification_ids] : pending_notifications_) {
      for(int i = 0; notification_ids.size(); i++) {
        if(notificationToUser(user, notification_ids[i]))
		std::cout << "MESSAGE COULD NOT BE SENT TO USER\n" << std::endl;
      }
    }
  }
}

bool Server::logoffUser(std::string username)
{
  if (users_.find(username) != users_.end()) { // user already exists on the db
    User& user = users_[username];;
    user.decrementSessions();
    return true;
  }

  return false;
}

bool Server::followUser(Follow follow)
{
  std::string curr_user = follow.client;
  std::string user_followed = follow.user_followed;

  if (users_.find(user_followed) != users_.end()) { // user exists on the db
    User& user = users_[user_followed];
    user.addFollower(curr_user);
    return true;
  }

  // return false 
  return false;
}

void Server::createConnection()
{
  pthread_t senderTid;
  pthread_t receiverTid;

  pthread_create(&senderTid, NULL, sendNotifications, (void *)this);
  pthread_create(&receiverTid, NULL, receiveCommand, (void *)this);
}
