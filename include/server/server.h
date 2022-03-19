#include "common.h"
#include "notification.h"
#include "user.h"
#include "ui.h"
#include <map>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <semaphore.h>
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
  // TEMP: set it as zero, change after implement DB
  long int new_notification_id_ = 0;
  // map that stores the logged users
  // mapping the users with the addresses that are used by them
  std::unordered_map<std::string, std::vector<struct sockaddr_in>>
      logged_users_;

  Ui ui;

  // attributes for server data
  int socket_;
  struct sockaddr_in server_address_;

  // attributes for mutex and semaphores
  pthread_mutex_t lock_;
  sem_t sem_full_;

  // constants
  const std::string db_file_name_ = "database.txt";

  // aux functions
  bool isLogged(const std::string &username);

public:
  Server();
  bool loginUser(std::string username, struct sockaddr_in user_address);
  bool logoffUser(std::string username);
  bool followUser(Follow follow);
  void addNotification(const Notification &notification);
  bool notificationToUser(const std::string &user, int notification_id);
  static void *sendNotifications(void *);
  static void *receiveCommand(void *);
  void createConnection();
  int sendCmdStatus(std::string status, char* confirmation_packet, struct sockaddr_in client_address);
  void sendStoredNotifications(std::string username);
  bool readDatabase();
  void addUserRelationToDB(std::string user, std::string follower);
};
