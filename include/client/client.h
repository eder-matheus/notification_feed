#include <string>

enum class CmdType;

class Client {
private:
  int id_;
  std::string username_;

public:
  Client();
  static void *commandToServer(void *);
  CmdType validateCommand(std::string input, std::string& content);
  static void *receiveFromServer(void *);
  void createConnection();
};
