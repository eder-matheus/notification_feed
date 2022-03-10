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
