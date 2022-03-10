#include <string>

class Notification {
private:
  int id_;
  int timestamp_;
  int length_;
  int pending_receivers_;
  std::string message_;
  std::string username_;

public:
  Notification() = default;
  Notification(std::string message, std::string username);
  int getTimestamp();
  int getLength();
  int getPendingReceivers();
  std::string getMessage();
  void decrementPendingReceivers();
};