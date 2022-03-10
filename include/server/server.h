#include "server.h"
#include "notification.h"
#include "user.h"
#include <string>
#include <unordered_map>

class Server {
private:
  std::unorderd_map<std::string, User> users_;
  std::unorderd_map<std::string, std::vector<Notification>> received_notifications_;

public:
  Server();

};
