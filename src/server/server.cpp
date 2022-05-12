#include "server.h"
#include <arpa/inet.h>
#include <cstring>
#include <fstream>
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
#include <unistd.h>
#include <algorithm>

void Server::sigintHandler(int sig_num) {
  signal(SIGINT, sigintHandler);
  std::cout << "Server closed.\n";
  exit(0);
}

Server::Server()
    : new_notification_id_(0), ui_(FileType::None), primary_id_(1) {}

bool Server::isLogged(const std::string &username) {
  return logged_users_.find(username) != logged_users_.end();
}

bool Server::loginUser(const std::string &username,
                       struct sockaddr_in user_address) {
  if (users_.find(username) !=
      users_.end()) { // user already exists on the data structures
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

bool Server::logoffUser(const std::string &username) {
  if (users_.find(username) !=
      users_.end()) { // user already exists on the data structures
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

bool Server::followUser(const Follow &follow) {
  const std::string &curr_user = follow.client;
  const std::string &user_followed = follow.user_followed;

  if (users_.find(user_followed) != users_.end() &&
      curr_user != user_followed) { // user exists on the data structures and it
                                    // is not the current user
    User &user = users_[user_followed];
    user.addFollower(curr_user);
    if (isPrimary()) {
      addUserRelationToDB(user_followed, curr_user);
    }
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
  char packet[BUFFER_SIZE]; // convert notification to string
  codificatePackage(packet, CmdType::ServerSend, notification.getMessage(),
                    notification.getTimestamp(), notification.getUsername());
  std::vector<struct sockaddr_in> client_addresses =
      logged_users_[user]; // get client addresses from map
  for (struct sockaddr_in &addr : client_addresses) {
    int n = sendto(socket_, packet, BUFFER_SIZE, 0, (struct sockaddr *)&addr,
                   sizeof(struct sockaddr));
    if (n < 0) {
      ui_.print(UiType::Error, "Cannot send to client.");
      return false;
    }
    // receive visualization confirmation
  }

  notifications_[notification_id].decrementPendingReceivers();
  updateSentNotification(notification_id);

  // delete notification from server if there is no pending receivers
  if (notifications_[notification_id].getPendingReceivers() == 0) {
    notifications_.erase(notification_id);
    ui_.print(UiType::Info,
              "Notification " + std::to_string(notification_id) + " deleted.");
  }

  return true;
}

void Server::sendStoredNotifications(const std::string &username) {
  if (isPrimary()) {
    auto itr = pending_notifications_.find(username);
    if (itr != pending_notifications_.end()) {
      std::vector<long int> pending = itr->second;

      for (int i = 0; i < pending.size(); i++) {
        notificationToUser((const std::string &)username, pending[i]);
      }
      pending_notifications_.erase(username);
    }
  }
}

bool Server::readDatabase() {
  std::string line;
  std::ifstream db(db_file_name_);
  if (db.is_open()) {
    while (std::getline(db, line)) {
      std::string user = line.substr(0, line.find(' '));
      line.erase(0, line.find(' ') + sizeof(char));
      std::string follower = line;
      if (users_.find(user) == users_.end()) {
        users_[user] = User(user);
      }
      users_[user].addFollower(follower);
    }
  } else {
    return false;
  }

  return true;
}

bool Server::readServersConfig() {
  std::string line;
  std::ifstream cfg(cfg_file_name_);
  if (cfg.is_open()) {
    while (std::getline(cfg, line)) {
      if (line[0] == '#')
        continue;
      int id = std::stoi(line.substr(0, line.find(' ')));
      topology_.push_back(id);
      line.erase(0, line.find(' ') + sizeof(char));
      int port = std::stoi(line);
      if (servers_ports_.find(id) == servers_ports_.end()) {
        servers_ports_[id] = port;
      }
    }
  } else {
    return false;
  }

  return true;
}

void Server::addUserRelationToDB(const std::string &user,
                                 const std::string &follower) {
  std::ofstream db;
  db.open(db_file_name_, std::ios::app);
  db << user << " " << follower << "\n";
  db.close();
}

void *Server::receiveCommand(void *args) {
  Server *_this = (Server *)args;
  _this->ui_.print(UiType::Info, "Read to receive commands.");
  struct sockaddr_in client_address;
  socklen_t client_length = sizeof(struct sockaddr_in);
  char packet[BUFFER_SIZE], confirmation_packet[BUFFER_SIZE];

  signal(SIGINT, sigintHandler);
  while (1) {
    // receive from client
    memset(packet, 0, BUFFER_SIZE);
    int n = recvfrom(_this->socket_, packet, BUFFER_SIZE, 0,
                     (struct sockaddr *)&(client_address), &(client_length));
    if (n < 0)
      _this->ui_.print(UiType::Error, "Cannot receive command from client.");

    std::vector<std::string> decoded_packet = decodificatePackage(packet);
    std::string received_command = decoded_packet[0];

    if (received_command == "send") {
      std::string message = decoded_packet[1];
      unsigned long int timestamp = std::stoul(decoded_packet[2], nullptr, 10);
      std::string username = decoded_packet[3];

      Notification received_notification(message, timestamp, username);
      pthread_mutex_lock(&_this->lock_);
      _this->addNotification(received_notification);
      _this->replicateRequests(packet);
      pthread_mutex_unlock(&_this->lock_);
      if (_this->isPrimary()) {
        sem_post(&_this->sem_full_);
      }
      _this->ui_.print(UiType::Success,
                       "Notification from " + username + " registered.");
      // update db
    } else if (received_command == "follow") {
      std::string followed_user = decoded_packet[1];
      std::string username = decoded_packet[2];
      Follow follow(username, followed_user);
      bool follow_ok = _this->followUser(follow);
      if (follow_ok) {
        _this->ui_.print(UiType::Success,
                         username + " followed " + followed_user + ".");
        _this->replicateRequests(packet);
      } else {
        _this->ui_.print(UiType::Error, followed_user + " not found.");
      }
    } else if (received_command == "login") {
      std::string username = decoded_packet[1];
      pthread_mutex_lock(&_this->lock_);
      bool login_ok = _this->loginUser(username, client_address);
      if (login_ok) {
        _this->ui_.print(UiType::Success, username + " logged.");
        _this->replicateRequests(packet);
        // send confirmation to client
        if (_this->sendCmdStatus(CMD_OK, confirmation_packet, client_address) <
            0) {
          _this->ui_.print(UiType::Error,
                           "Cannot send login confirmation to client.\n");
        }
        _this->sendStoredNotifications(username);
        _this->fixClientPort(username, client_address);
      } else {
        _this->ui_.print(UiType::Error,
                         username + " has reached max sessions.");
        // send confirmation to client
        if (_this->sendCmdStatus(CMD_FAIL, confirmation_packet,
                                 client_address) < 0) {
          _this->ui_.print(UiType::Error,
                           "Cannot send login confirmation to client.\n");
        }
      }
      pthread_mutex_unlock(&_this->lock_);
    } else if (received_command == "logoff") {
      std::string username = decoded_packet[1];
      _this->logoffUser(username);
      _this->replicateRequests(packet);
      _this->ui_.print(UiType::Success,
                       username + " succesfully disconnected.");
    } else if (received_command == "fix_port") {
      // only non-primary serverrs handle FixPort commands
      if (!_this->isPrimary()) {
        std::string username = decoded_packet[1];
        int port = std::stoi(decoded_packet[2]);
        int sessions = _this->logged_users_[username].size();
        _this->logged_users_[username][sessions - 1].sin_port = port;
      }
    } else if (received_command == "update_notification") {
      if (!_this->isPrimary()) {
        int notification_id = std::stoi(decoded_packet[1]);
        _this->notifications_[notification_id].decrementPendingReceivers();

        // delete notification from server if there is no pending receivers
        if (_this->notifications_[notification_id].getPendingReceivers() == 0) {
          _this->notifications_.erase(notification_id);
          _this->ui_.print(UiType::Info, "Notification " +
                                             std::to_string(notification_id) +
                                             " deleted.");
        }
      }
    } else if (received_command == "ring_ack") {
      sem_post(&_this->sem_ring_);
    } else if (received_command == "ring_cmd") {
      _this->ring_list_.clear();
      _this->ring_sender_port_ =
          _this->servers_ports_[std::stoi(decoded_packet[2])];
      _this->ring_status_ = ring_commands[decoded_packet[1]];
      for (char const &id : decoded_packet[3]) {
        if (id != ' ') {
          _this->ring_list_.push_back(id - '0');
        }
      }
      sem_post(&_this->sem_ring_);
    } else {
      _this->ui_.print(UiType::Error,
                       "Command not identified: " + std::string(packet) + ".");
    }
  }
  return 0;
}

void *Server::sendNotifications(void *args) {
  Server *_this = (Server *)args;
  _this->ui_.print(UiType::Info, "Read to send notifications.");
  std::vector<std::string> logged_users;
  while (true) {
    sem_wait(&_this->sem_full_);
    if (_this->id_ == _this->primary_id_) {
      pthread_mutex_lock(&_this->lock_);
      for (auto &notification : _this->pending_notifications_) {
        const auto &user = notification.first;
        if (_this->isLogged(user)) {
          logged_users.push_back(user);
          auto &notification_ids = notification.second;
          for (int i = 0; i < notification_ids.size(); i++) {
            if (!_this->notificationToUser(user, notification_ids[i])) {
              _this->ui_.print(UiType::Error, "Notification not sent.");
            } else {
              _this->ui_.print(UiType::Success, "Notification sent.");
            }
          }
        }
      }
      for (std::string user : logged_users) {
        _this->pending_notifications_.erase(user);
      }
      logged_users.clear();
      pthread_mutex_unlock(&_this->lock_);
    }
  }
}

void *Server::electionThread(void *args) {
  initRingCommands();
  Server *_this = (Server *)args;
  int last_try = -1;
  struct hostent *server = gethostbyname("localhost");
  struct sockaddr_in rm_address;
  char packet[BUFFER_SIZE];
  bool update_list = false;
  bool purge_old_list = false;
  std::string package_content;
  int s;
  struct timespec ts;

  if (clock_gettime(CLOCK_REALTIME, &ts) == -1)
    std::exit(-1);

  rm_address.sin_family = AF_INET;
  rm_address.sin_addr = *((struct in_addr *)server->h_addr);
  bzero(&(rm_address.sin_zero), 8);

  _this->ui_.print(UiType::Info, "Ready to run the ring alg.");
  // create contact packet ("ring_cmd\nCmdType::New\nmy_id\nid_\n")
  _this->ring_status_ = CmdType::NewServer;
  _this->active_list_.push_back(_this->id_);
  std::string my_id = std::to_string(_this->id_);
  my_id.append("\n");
  my_id.append(my_id);
  my_id.append("\n");
  codificatePackage(packet, _this->ring_status_, my_id);

  bool made_contact = false;
  while (!made_contact) {
    last_try = findFirstNeighboor(_this->id_, last_try, _this->topology_);
    if (last_try == -2) {
      made_contact = true;
      _this->primary_id_ = _this->id_;
    } else {
      int port = _this->servers_ports_[last_try];
      rm_address.sin_port = htons(port);
      int n = sendto(_this->socket_, packet, strlen(packet), 0,
                     (const struct sockaddr *)&rm_address,
                     sizeof(struct sockaddr_in));
      if (n < 0) {
        _this->ui_.print(UiType::Error, "Failed to send message to neighboor");
      }
      ts.tv_sec += 1;
      while ((s = sem_timedwait(&_this->sem_ring_, &ts)) == -1 &&
             errno == EINTR)
        continue;
      if (s == -1) {
        if (errno == ETIMEDOUT)
          made_contact = false;
      } else
        made_contact = true;
    }
  }
  while (true) {
    update_list = false;
    made_contact = false;
    purge_old_list = false;
    sem_wait(&_this->sem_ring_);
    _this->ring_status_ = CmdType::AckRing;
    codificatePackage(packet, _this->ring_status_, std::to_string(1));
    rm_address.sin_port = htons(_this->ring_sender_port_);
    int n = sendto(_this->socket_, packet, strlen(packet), 0,
                   (const struct sockaddr *)&rm_address,
                   sizeof(struct sockaddr_in));
    if (n < 0) {
      _this->ui_.print(UiType::Error, "Failed to ack neighboor");
    }

    _this->ring_status_ =
        ringIter(_this->id_, _this->active_list_, _this->ring_list_,
                 _this->ring_status_, update_list);
    // ring status needs to cover everything that can happen with the ring
    if (_this->ring_status_ == CmdType::NewServer ||
        _this->ring_status_ == CmdType::MonitorNew) {
      _this->active_list_ = _this->ring_list_;
      _this->active_list_.push_back(_this->id_);
      if (_this->ring_status_ == CmdType::MonitorNew) {
        // will not send anything since another list is already in the ring
        purge_old_list = true;
      }
    }
    if (_this->ring_status_ == CmdType::NormalRing && purge_old_list) {
      _this->ring_list_.clear();
    }
    if (_this->ring_status_ == CmdType::NormalRing && update_list) {
      _this->active_list_ = _this->ring_list_;
    }
    if (_this->ring_status_ == CmdType::ElectLeader) {
      _this->active_list_ = _this->ring_list_;
      _this->active_list_.push_back(_this->id_);
    }
    if (_this->ring_status_ == CmdType::FindLeader) {
      _this->active_list_ = _this->ring_list_;
      _this->primary_id_ = electPrimary(_this->active_list_);
    }

    package_content = std::to_string(_this->id_);
    package_content.append("\n");
    for (int active_id : _this->active_list_) {
      package_content.append(std::to_string(active_id));
      package_content.append(" ");
    }
    codificatePackage(packet, _this->ring_status_, package_content);

    while (!made_contact && !purge_old_list) {
      int next = getNextId(_this->id_, _this->active_list_);
      if (next == -1) {
        _this->primary_id_ = _this->id_;
        made_contact = true;
      } else {
        rm_address.sin_port = htons(_this->servers_ports_[next]);
        n = sendto(_this->socket_, packet, strlen(packet), 0,
                   (const struct sockaddr *)&rm_address,
                   sizeof(struct sockaddr_in));
        if (n < 0) {
          _this->ui_.print(UiType::Error, "Failed to send list to next");
        }

        while ((s = sem_timedwait(&_this->sem_ring_, &ts)) == -1 &&
               errno == EINTR)
          continue;
        if (s == -1) {
          if (errno == ETIMEDOUT)
            made_contact = false;
        } else
          made_contact = true;

        if (!made_contact) {
	  std::vector<int>::iterator it = std::find(_this->active_list_.begin(), _this->active_list_.end(), next);
	  //int i = it - _this->active_list_.begin();
	  _this->active_list_.erase(it);
          //_this->active_list_.erase(std::remove(_this->active_list_.begin(),
          //                                      _this->active_list_.end(),
          //                                      next),
          //                          _this->active_list_.end());
          if (next == _this->primary_id_ ||
              _this->ring_status_ == CmdType::FindLeader) {
            _this->ring_status_ = CmdType::ElectLeader;
            // _this->active_list_.clear();
            // _this->active_list_.push_back(id_);
            package_content.clear();
            package_content = std::to_string(_this->id_);
            package_content.append("\n");
            package_content.append(std::to_string(_this->id_));
            package_content.append("\n");
            codificatePackage(packet, _this->ring_status_, package_content);
          }
        }
      }
    }
    package_content.clear();
  }
}

void Server::createConnection(int id) {
  if ((socket_ = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
    ui_.print(UiType::Error, "Cannot open socket.");
    exit(1);
  }

  if (!readDatabase()) {
    ui_.print(UiType::Warn, "File " + db_file_name_ + " not found.");
  }

  if (!readServersConfig()) {
    ui_.print(UiType::Warn, "File " + cfg_file_name_ + " not found.");
  }

  id_ = id;
  ui_.print(UiType::Success, "Server " + std::to_string(id_) +
                                 " started using port " +
                                 std::to_string(servers_ports_[id_]) + ".");

  server_address_.sin_family = AF_INET;
  server_address_.sin_port = htons(servers_ports_[id_]);
  server_address_.sin_addr.s_addr = INADDR_ANY;
  bzero(&(server_address_.sin_zero), 8);

  if (bind(socket_, (struct sockaddr *)&server_address_,
           sizeof(struct sockaddr)) < 0)
    ui_.print(UiType::Error,
              "Cannot perform binding on server " + std::to_string(id_) + ".");

  sem_init(&sem_ring_, 0, 0);
  sem_init(&sem_full_, 0, 0);
  pthread_mutex_init(&lock_, NULL);

  pthread_t senderTid;
  pthread_t receiverTid;
  pthread_t electionTid;

  pthread_create(&receiverTid, NULL, receiveCommand, (void *)this);
  pthread_create(&senderTid, NULL, sendNotifications, (void *)this);
  pthread_create(&electionTid, NULL, electionThread, (void *)this);

  pthread_join(receiverTid, NULL);
  pthread_join(senderTid, NULL);
  pthread_join(electionTid, NULL);

  pthread_mutex_destroy(&lock_);
}

int Server::sendCmdStatus(const std::string &status, char *confirmation_packet,
                          struct sockaddr_in client_address) {
  int n = 0;
  if (isPrimary()) {
    memset(confirmation_packet, 0, BUFFER_SIZE);
    codificatePackage(confirmation_packet, CmdType::Confirmation, status);
    n = sendto(socket_, confirmation_packet, strlen(confirmation_packet), 0,
               (struct sockaddr *)&(client_address),
               sizeof(struct sockaddr_in));
  }
  return n;
}

void Server::replicateRequests(char *packet) {
  if (isPrimary()) {
    int n = -1;
    struct hostent *server = gethostbyname("localhost");
    struct sockaddr_in rm_address;

    rm_address.sin_family = AF_INET;
    rm_address.sin_addr = *((struct in_addr *)server->h_addr);
    bzero(&(rm_address.sin_zero), 8);

    for (auto &server_port : servers_ports_) {
      int id = server_port.first;
      int port = server_port.second;

      if (id != id_) {
        rm_address.sin_port = htons(port);
        n = sendto(socket_, packet, strlen(packet), 0,
                   (const struct sockaddr *)&rm_address,
                   sizeof(struct sockaddr_in));
        if (n < 0) {
          ui_.print(UiType::Error, "Failed to replicate request.");
        }
      }
    }
  }
}

void Server::fixClientPort(std::string username,
                           struct sockaddr_in user_address) {
  if (isPrimary()) {
    char packet[BUFFER_SIZE];
    int n = -1;
    struct hostent *server = gethostbyname("localhost");
    struct sockaddr_in rm_address;

    rm_address.sin_family = AF_INET;
    rm_address.sin_addr = *((struct in_addr *)server->h_addr);
    bzero(&(rm_address.sin_zero), 8);

    codificatePackage(packet, CmdType::FixPort,
                      std::to_string(user_address.sin_port), 0, username);

    for (auto &server_port : servers_ports_) {
      int id = server_port.first;
      int port = server_port.second;

      if (id != id_) {
        rm_address.sin_port = htons(port);
        n = sendto(socket_, packet, strlen(packet), 0,
                   (const struct sockaddr *)&rm_address,
                   sizeof(struct sockaddr_in));
        if (n < 0) {
          ui_.print(UiType::Error, "Failed to replicate request.");
        }
      }
    }
  }
}

void Server::updateSentNotification(int notification_id) {
  if (isPrimary()) {
    char packet[BUFFER_SIZE];
    int n = -1;
    struct hostent *server = gethostbyname("localhost");
    struct sockaddr_in rm_address;

    rm_address.sin_family = AF_INET;
    rm_address.sin_addr = *((struct in_addr *)server->h_addr);
    bzero(&(rm_address.sin_zero), 8);

    codificatePackage(packet, CmdType::UpdateNotification,
                      std::to_string(notification_id));

    for (auto &server_port : servers_ports_) {
      int id = server_port.first;
      int port = server_port.second;

      if (id != id_) {
        rm_address.sin_port = htons(port);
        n = sendto(socket_, packet, strlen(packet), 0,
                   (const struct sockaddr *)&rm_address,
                   sizeof(struct sockaddr_in));
        if (n < 0) {
          ui_.print(UiType::Error, "Failed to replicate request.");
        }
      }
    }
  }
}

void Server::sendBackupNotifications() {
  std::vector<std::string> logged_users;
  for (auto &notification : pending_notifications_) {
    const auto &user = notification.first;
    if (isLogged(user)) {
      logged_users.push_back(user);
      auto &notification_ids = notification.second;
      for (int i = 0; i < notification_ids.size(); i++) {
        if (!notificationToUser(user, notification_ids[i])) {
          ui_.print(UiType::Error, "Notification not sent.");
        } else {
          ui_.print(UiType::Success, "Notification sent.");
        }
      }
    }
  }

  for (std::string user : logged_users) {
    pending_notifications_.erase(user);
  }
  logged_users.clear();
}
