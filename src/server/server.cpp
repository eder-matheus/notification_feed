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