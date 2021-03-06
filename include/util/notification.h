#include <string>

class Notification {
private:
  long int id_;
  unsigned long int timestamp_;
  int length_;
  int pending_receivers_;
  std::string message_;
  std::string username_;

public:
  Notification() = default;
  Notification(const std::string &message, const std::string &username);
  Notification(const std::string &message, unsigned long int timestamp,
               const std::string &username);
  int getId() const;
  void setId(long int id);
  unsigned long int getTimestamp() const;
  void setTimestamp(unsigned long int timestamp);
  int getLength();
  int getPendingReceivers();
  void setPendingReceivers(int pending_receivers);
  std::string getMessage() const;
  std::string getUsername() const;
  void decrementPendingReceivers();
  void print();
};
