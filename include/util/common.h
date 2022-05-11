#include <string>
#include <vector>

#define MAX_SESSIONS 2
#define PORT 4000
#define BUFFER_SIZE 200
#define REC_WAIT_LIMIT 4
#define REC_WAIT 1
#define CMD_OK "1\n"
#define CMD_FAIL "0\n"
#define CMD_404 "9\n"

enum class CmdType {
  Send,
  Follow,
  Login,
  Receive,
  Logoff,
  Confirmation,
  FixPort,
  UpdateNotification,
  Error
};

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

void codificatePackage(char *package, CmdType type, const std::string &information,
                       unsigned long int timestamp = 0, std::string user = "0");
