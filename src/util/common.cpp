#include "common.h"
#include <cstring>
#include <string>
#include <vector>

// Notification package:  send message timestamp username
// Follow package:        follow followed_user username
// Login package:         login username
// Receive package:       message timestamp username
// Logoff package:        logoff username
std::vector<std::string> decodificatePackage(char *package) {
  std::vector<std::string> broken_package;
  std::string full_package(package);
  std::size_t space_pos;
  std::string data = "";
  bool decoded = false;

  while (!decoded) {
    space_pos = full_package.find(' ');
    if (space_pos != std::string::npos) {
      data = full_package.substr(0, space_pos);
      broken_package.push_back(data);
      full_package.erase(0, full_package.find(' ') + sizeof(char));
    } else {
      decoded = true;
    }
  }

  return broken_package;
}

void codificatePackage(char *package, CmdType type, std::string information,
                       unsigned long int timestamp, std::string user) {

  std::string raw_information = information.substr(0, information.find(' '));
  raw_information.append(" ");

  std::string time_string = std::to_string(timestamp);
  time_string.append(" ");

  std::string raw_user = user.substr(0, user.find('\n'));
  raw_user.append(" ");

  if (type == CmdType::Send) {
    std::strcpy(package, "send ");
    std::strcat(package, raw_information.c_str());
    std::strcat(package, time_string.c_str());
    std::strcat(package, raw_user.c_str());
  } else if (type == CmdType::Follow) {
    std::strcpy(package, "follow ");
    std::strcat(package, raw_information.c_str());
    std::strcat(package, user.c_str());
  } else if (type == CmdType::Login) {
    std::strcpy(package, "login ");
    std::strcat(package, raw_information.c_str());
  } else if (type == CmdType::Receive) {
    std::strcpy(package, raw_information.c_str());
    std::strcat(package, time_string.c_str());
    std::strcat(package, raw_user.c_str());
  } else if (type == CmdType::Logoff) {
    std::strcpy(package, "logoff ");
    std::strcat(package, raw_information.c_str());
  } else if (type == CmdType::Confirmation) {
    std::strcpy(package, raw_information.c_str());
  }
}
