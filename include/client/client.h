#include "ui.h"
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>

enum class CmdType;

class Client {
private:
  int id_;
  std::string username_;
  bool ready_to_receive_;

  // attributes for client socket
  int socket_;
  struct timeval socket_time_;
  struct sockaddr_in server_address_, from_;
  struct hostent *server_;

  Ui ui_;

public:
  static void sigintHandler(int sig_num);
  Client() = default;
  Client(const std::string &username);
  static void *commandToServer(void *args);
  CmdType validateCommand(std::string &input, std::string &content);
  static void *receiveFromServer(void *args);
  void createConnection(char *server, const std::string &gate);
  std::string checkServerAnswer();
  std::string tryCommand(char *packet, int time_limit, bool check_answer);
};
