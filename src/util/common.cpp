#include "common.h"


std::vector<std::string> decodificatePackage(char* package) {
 
  std::vector<std::string> broken_package;
  std::string full_package(package);
  std::size_t space_pos;
  bool decoded = false;

  while(!decoded) {
    space_pos = full_package.find(' ');
    if(space_pos!=std::string::npos) {
      std::string data = full_package.substr(0, space_pos);
      broken_package.push_back(data);
      full_package.erase(0, full_package.find(' ') + sizeof(char));
    } else {
      std::strind data = full_package.substr(0, '\n');
      broken_package.push_back(data);
      decoded = true;
    }
  }

  return broken_package;
}

char* codificatePackage(CmdType type, std::string information, int timestamp, std::string user) {
  
   char package[200];

   std::string raw_information = information.substr(0, information.find('\n'));
   std::strcat(raw_information, " ");
   
   std::string time_string = std::to_string(timestamp);
   std::strcat(time_string, " ");
  
   std::string raw_user = user.substr(0, user.find('\n'));
   std::strcat(raw_user, "\n");

   if (type == CmdType::Send) {
     std::strcpy(package, "send ");
     std::strcat(package, raw_information);
     std::strcat(package, time_string);
     std::strcat(package, raw_user)
   } else if (type == CmdType::Follow) {
     std::strcpy(package, "follow ");
     std::strcat(package, raw_information);
   } else if (type == CmdType::Login) {
     std::strcpy(package, "login ");
     std::strcat(package, raw_information);
   } else if (type == CmdType::Receive) {
     std::strcpy(package, raw_information);
     std::strcat(package, time_string);
     std::strcat(package, raw_user);
   }

  return package;
}	
