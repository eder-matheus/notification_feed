//#include "notification.h"
//#include "definitions.h"
//#include "user.h"
#include "../../include/util/notification.h"
#include "../../include/util/definitions.h"
#include "../../include/server/user.h"
#include <string>
#include <map>
#include <unordered_map>

class Server {
private:
  std::unordered_map<std::string, User> users_;
  // this map stores all the notifications created by all users
  std::unordered_map<long int, Notification> notifications_;
  // this map stores the notifications that the user (key) have to receive
  // the vector of notification ids should be sorted in crescent way
  std::map<std::string, std::vector<long int>> pending_notifications_;
  // this id is used to define the ids for new notifications added
  // should be stored on the database
  long int new_notification_id_;

public:
  Server();
  bool loginUser(std::string username);
  void addNotification(const Notification& notification);
  bool notificationToUser(std::string user, int notification_id);
  static void* sendNotifications(void *);
  static void* receiveCommand(void *);
  bool logoffUser(std::string username);
  bool followUser(Follow follow);
  void createConnection();
};
