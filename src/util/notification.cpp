#include "notification.h"
#include <chrono>
#include <iostream>
#include <string>

Notification::Notification(std::string message, std::string username)
    : id_(-1), length_(message.size()), pending_receivers_(-1),
      message_(message), username_(username) {
  using namespace std::chrono;
  timestamp_ =
      duration_cast<milliseconds>(system_clock::now().time_since_epoch())
          .count();
};

Notification::Notification(std::string message, unsigned long int timestamp,
                           std::string username)
    : id_(-1), timestamp_(timestamp), length_(message.size()),
      pending_receivers_(-1), message_(message), username_(username){};

int Notification::getId() const { return id_; }

void Notification::setId(long int id) { id_ = id; }

int Notification::getTimestamp() const { return timestamp_; }

void Notification::setTimestamp(int timestamp) { timestamp_ = timestamp; }

int Notification::getLength() { return length_; }

int Notification::getPendingReceivers() { return pending_receivers_; }

void Notification::setPendingReceivers(int pending_receivers) {
  pending_receivers_ = pending_receivers;
}

std::string Notification::getMessage() const { return message_; }

std::string Notification::getUsername() const { return username_; }

void Notification::decrementPendingReceivers() { pending_receivers_--; }

void Notification::print() {
  std::cout << username_ << ": " << message_ << " (" << timestamp_ << ")\n";
}
