#include <string>

enum class CmdType;

class Client {
private:
  int id_;
  std::string username_;

public:
  Client() = default;
  Client(std::string username);
  static void *commandToServer(void *);
  CmdType validateCommand(std::string input, std::string& content);
  static void *receiveFromServer(void *);
  void createConnection();
};
