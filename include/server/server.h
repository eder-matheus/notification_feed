#include "server.h"
#include "notification.h"
#include "user.h"
#include <string>
#include <map>
#include <unordered_map>

class Server {
private:
  std::unorderd_map<std::string, User> users_;
  // this map stores all the notifications created by all users
  std::unorderd_map<long int, Notification> notifications_;
  // this map stores the notifications that the user (key) have to receive
  // the vector of notification ids should be sorted in crescent way
  std::map<std::string, std::vector<long int>> pending_notifications_;

public:
  Server();
  bool loginUser(std::string username);
};
