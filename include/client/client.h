#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>

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

public:
  static void sigintHandler(int sig_num);
  Client() = default;
  Client(std::string username);
  static void *commandToServer(void *);
  CmdType validateCommand(std::string input, std::string &content);
  static void *receiveFromServer(void *);
  void createConnection(char *, std::string);
  std::string checkServerAnswer();
  std::string tryCommand(char *, int, bool);
};
