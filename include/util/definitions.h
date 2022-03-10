#define CMD_SEND "SEND"
#define CMD_FOLLOW "FOLLOW"
#define MAX_SESSIONS 2

struct Follow
{
  std::string client;
  std::string user_followed;
};