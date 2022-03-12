#include <string>

class Notification {
private:
  long int id_;
  int timestamp_;
  int length_;
  int pending_receivers_;
  std::string message_;
  std::string username_;

public:
  Notification() = default;
  Notification(std::string message, std::string username);
  int getId();
  void setId(long int id);
  int getTimestamp() const;
  void setTimestamp(int timestamp);
  int getLength();
  int getPendingReceivers();
  void setPendingReceivers(int pending_receivers);
  std::string getMessage() const;
  std::string getUsername() const;
  void decrementPendingReceivers();
  void print();
};
