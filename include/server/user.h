#include <string>
#include <vector>

class User {
private:
  std::string username_;
  std::vector<string> followers_;
  int sessions_;

public:
  User(std::string username);
  int getSessions() { return sessions_; }
  void incrementSessions() { sessions_++; }
};
