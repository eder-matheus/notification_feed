#include <chrono>
#include <string>
#include "notification.h"


Notification::Notification(std::string message, std::string username)
  : id_(-1),
    length_(message.size()),
    pending_receivers_(-1),
    message_(message),
    username_(username)
{
  using namespace std::chrono;
  timestamp_ = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
};

int Notification::getTimestamp() {
  return timestamp_;
}

int Notification::getLength() {
  return length_;
}

int Notification::getPendingReceivers() {
  return pending_receivers_;
}

std::string Notification::getMessage() {
  return message_;
}

void Notification::decrementPendingReceivers() {
  pending_receivers_--;
}