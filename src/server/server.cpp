//#include "server.h"
#include "../../include/server/server.h"
#include <iostream>
#include <pthread.h>
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

void Server::addNotification(const Notification& notification) {
  // add new notification to the notifications map
  notifications_[new_notification_id_] = notification;
  notifications_[new_notification_id_].setId(new_notification_id_);

  // add the notification to the list of pending notifications
  // for the followers of the user who created the notification
  const std::vector<std::string>& followers = users_[notification.getUsername()].getFollowers();
  for (const std::string& follower : followers) {
    pending_notifications_[follower].push_back(new_notification_id_);
  }
  notifications_[new_notification_id_].setPendingReceivers(followers.size());

  // increment new_notification_id_ to have a valid id for the next added notification
  new_notification_id_++;
}

bool Server::notificationToUser(std::string user, int notification_id) {
  //send notification
  //receive visualization confirmation
  //notifications_[notification_id].decrementPendingReceivers();
  return true;
  //else return false if could not send
}

void* Server::receiveCommand(void *args) {
  
  CmdType received_command;
  //receive command from client

  if(received_command == CmdType::Send) {
    Notification received_notification;
    //receive notification from client
    //addNotification(received_notification);
    //change db
  } else if(received_command == CmdType::Follow) {
    Follow follow("ed", "er");
    //receive follow from client
    //followUser(follow);
    //change db
  }
  return 0;
}

void* Server::sendNotifications(void *args) {
  Server *_this = (Server *)args;
  while(true) {
    for(auto& notification : _this->pending_notifications_) {
      auto& user = notification.first;
      auto& notification_ids = notification.second;
      for(int i = 0; notification_ids.size(); i++) {
        if(!_this->notificationToUser(user, notification_ids[i]))
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
