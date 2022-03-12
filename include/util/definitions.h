#define MAX_SESSIONS 2

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