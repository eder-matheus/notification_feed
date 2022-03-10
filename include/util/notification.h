#include <string>
#include <vector>

class Notification {
private:
  int id_;
  int timestamp_;
  int length_;
  int pending_receivers_;
  std::string message_;

public:
  int getTimestamp() { return timestamp_; }
  int getLength() { return length_; }
  int getPendingReceivers() { return pending_receivers_; }
  std::string getMessage() { return message_; }
  void decrementPendingReceivers() {pending_receivers_--; }
};