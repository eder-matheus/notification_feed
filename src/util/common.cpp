#include "common.h"
#include <cstring>
#include <iostream>
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
  bool decoded = false;

  while (!decoded) {
    std::size_t eol_pos = full_package.find('\n');
    if (eol_pos != std::string::npos) {
      std::string data = full_package.substr(0, eol_pos);
      broken_package.push_back(data);
      full_package.erase(0, full_package.find('\n') + sizeof(char));
    } else {
      decoded = true;
    }
  }

  return broken_package;
}

void codificatePackage(char *package, CmdType type,
                       const std::string &information,
                       unsigned long int timestamp, std::string user) {

  std::string raw_information;
  std::string raw_information_1;
  std::string raw_information_2;

  if (type == CmdType::NewServer || type == CmdType::MonitorNew || type == CmdType::NormalRing ||
      type == CmdType::ElectLeader || type == CmdType::FindLeader) {
    std::string temp_information = information;
    raw_information_1 = temp_information.substr(0, temp_information.find(' '));
    raw_information_1.append("\n");
    temp_information.erase(0, temp_information.find(' ') + sizeof(char));
    raw_information_2 = temp_information.substr(0, temp_information.find('\n'));
    raw_information_2.append("\n");
  }
  else {
    raw_information = information.substr(0, information.find('\n'));
    raw_information.append("\n");
  }
  std::string time_string = std::to_string(timestamp);
  time_string.append("\n");

  std::string raw_user = user.substr(0, user.find('\n'));
  raw_user.append("\n");

  if (type == CmdType::Send) {
    std::strcpy(package, "send\n");
    std::strcat(package, raw_information.c_str());
    std::strcat(package, time_string.c_str());
    std::strcat(package, raw_user.c_str());
  } else if (type == CmdType::ServerSend) {
    std::strcpy(package, "server_send\n");
    std::strcat(package, raw_information.c_str());
    std::strcat(package, time_string.c_str());
    std::strcat(package, raw_user.c_str());
  } else if (type == CmdType::Follow) {
    std::strcpy(package, "follow\n");
    std::strcat(package, raw_information.c_str());
    std::strcat(package, raw_user.c_str());
  } else if (type == CmdType::Login) {
    std::strcpy(package, "login\n");
    std::strcat(package, raw_information.c_str());
  } else if (type == CmdType::Receive) {
    std::strcpy(package, raw_information.c_str());
    std::strcat(package, time_string.c_str());
    std::strcat(package, raw_user.c_str());
  } else if (type == CmdType::Logoff) {
    std::strcpy(package, "logoff\n");
    std::strcat(package, raw_information.c_str());
  } else if (type == CmdType::Confirmation) {
    std::strcpy(package, raw_information.c_str());
  } else if (type == CmdType::FixPort) {
    std::strcpy(package, "fix_port\n");
    std::strcat(package, raw_user.c_str());
    std::strcat(package, raw_information.c_str());
  } else if (type == CmdType::UpdateNotification) {
    std::strcpy(package, "update_notification\n");
    std::strcat(package, raw_information.c_str());
  } else if (type == CmdType::NewServer) {
    std::strcpy(package, "ring_cmd\n");
    std::strcat(package, "r_new\n");
    std::strcat(package, raw_information_1.c_str());
    std::strcat(package, raw_information_2.c_str());
  } else if (type == CmdType::MonitorNew) {
    std::strcpy(package, "ring_cmd\n");
    std::strcat(package, "r_moni\n");
    std::strcat(package, raw_information_1.c_str());
    std::strcat(package, raw_information_2.c_str());
  } else if (type == CmdType::NormalRing) {
    std::strcpy(package, "ring_cmd\n");
    std::strcat(package, "r_norm\n");
    std::strcat(package, raw_information_1.c_str());
    std::strcat(package, raw_information_2.c_str());
  } else if (type == CmdType::ElectLeader) {
    std::strcpy(package, "ring_cmd\n");
    std::strcat(package, "r_elect\n");
    std::strcat(package, raw_information_1.c_str());
    std::strcat(package, raw_information_2.c_str());
  } else if (type == CmdType::FindLeader) {
    std::strcpy(package, "ring_cmd\n");
    std::strcat(package, "r_lead\n");
    std::strcat(package, raw_information_1.c_str());
    std::strcat(package, raw_information_2.c_str());
  } else if (type == CmdType::AckRing) {
    std::strcpy(package, "ring_ack\n");
  } else if (type == CmdType::SetLeader) {
    std::strcpy(package, "set_leader\n");
    std::strcat(package, raw_information.c_str());
  }
}
