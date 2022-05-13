#include <netdb.h>
#include <netinet/in.h>
#include <semaphore.h>
#include <string>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <map>
#include "ui.h"

class FrontEnd {
private:
  int socket_;
  struct sockaddr_in front_end_address_;
  struct sockaddr_in client_address_;
  struct sockaddr_in server_address_;
  std::map<int, int> servers_ports_;
  int primary_server_id_;

  // constants
  const std::string cfg_file_name_ = "servers_config.cfg";

  Ui ui_;

  static void sigintHandler(int sig_num);

public:
  FrontEnd();
  bool readServers();
  void setPrimaryServer(int id);
  void createConnection(const std::string& server_name, const std::string& port);
  static void *receive(void *args);
  bool fromServer(const std::string& command);
};
