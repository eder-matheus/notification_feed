#include <string>

enum class CmdType;

class Client {
private:
  int id;

public:
  Client();
  static void *commandToServer(void *);
  CmdType validateCommand(std::string);
  static void *receiveFromServer(void *);
  void createConnection();
};
