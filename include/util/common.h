#include <string>
#include <vector>

#define MAX_SESSIONS 2
#define PORT 4000
#define BUFFER_SIZE 200
#define C_BUFFER_SIZE 17
#define CONFIRMATION_STRING "got your packet\n"

enum class CmdType { Send, Follow, Login, Receive, Logoff, Error };

struct Follow {
  std::string client;
  std::string user_followed;

  Follow() = default;
  Follow(std::string c, std::string uf) {
    client = c;
    user_followed = uf;
  }
};

std::vector<std::string> decodificatePackage(char *package);

void codificatePackage(char *, CmdType, std::string, int timestamp = -1,
                       std::string user = "0");
