#define MAX_SESSIONS 2
#define PORT 4000
#define BUFFER_SIZE 200

enum class CmdType
{
  Send,
  Follow,
  Login,
  Error
};

struct Follow
{
  std::string client;
  std::string user_followed;

  Follow(std::string c, std::string uf) {
    client = c;
    user_followed = uf;
  }
};