#include <string>
#include <vector>

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
  Notification(std::string message, std::string username)
    : id_(-1),
      length_(message.size()),
      pending_receivers_(-1),
      message_(message),
      username_(username)
  {};
  int getTimestamp() { return timestamp_; }
  int getLength() { return length_; }
  int getPendingReceivers() { return pending_receivers_; }
  std::string getMessage() { return message_; }
  void decrementPendingReceivers() {pending_receivers_--; }
};