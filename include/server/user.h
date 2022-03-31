#include <string>
#include <vector>

class User {
private:
  std::string username_;
  std::vector<std::string> followers_;
  int sessions_;

public:
  User() = default;
  User(const std::string &username);
  int getSessions() { return sessions_; }
  void incrementSessions() { sessions_++; }
  void decrementSessions() { sessions_--; }
  const std::vector<std::string> &getFollowers() { return followers_; }
  void addFollower(const std::string &username) { followers_.push_back(username); }
};
