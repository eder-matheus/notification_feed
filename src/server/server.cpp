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

int notificationToUser(string user, notification_id) {
  
  //send notification
  notifications_[notification_id].decrementPendingRecievers;
  return 0;
  //else return -1 if could not send
}

void* receiveCommand() {
  
  //receive enum of command (Follow or Send) from client
  //prepare to receive follow struct or notification object
  //change database and local structure
}

void* sendNotifications() {

  while(true) {
    for(auto& [user, notification_ids] : pending_notifications_) {
      for(int i = 0; notification_ids.size(); i++) {
        if(notificationToUser(user, notification_ids[i]))
		std::cout << "MESSAGE COULD NOT BE SENT TO USER\n" << std::endl;
      }
    }
  }
}
