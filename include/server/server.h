#include "common.h"
#include "notification.h"
#include "user.h"
#include <map>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>

class Server {
private:
  // attributes to store the metadata
  std::unordered_map<std::string, User> users_;
  // this map stores all the notifications created by all users
  std::unordered_map<long int, Notification> notifications_;
  // this map stores the notifications that the user (key) have to receive
  // the vector of notification ids should be sorted in crescent way
  std::map<std::string, std::vector<long int>> pending_notifications_;
  // this id is used to define the ids for new notifications added
  // should be stored on the database
  long int new_notification_id_;
  // map that stores the logged users
  // mapping the users with the addresses that are used by them
  std::unordered_map<std::string, std::vector<struct sockaddr_in>>
      logged_users_;

  // attributes for server data
  int socket_;
  socklen_t client_length_;
  struct sockaddr_in server_address_, client_address_;

  // aux functions
  bool isLogged(const std::string &username);

public:
  Server();
  bool loginUser(std::string username);
  bool logoffUser(std::string username);
  bool followUser(Follow follow);
  void addNotification(const Notification &notification);
  bool notificationToUser(const std::string &user, int notification_id);
  static void *sendNotifications(void *);
  static void *receiveCommand(void *);
  void createConnection();
};
