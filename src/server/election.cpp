#include "election.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

std::unordered_map<std::string, CmdType> ring_commands;

void initRingCommands() {
  ring_commands["r_norm"] = CmdType::NormalRing;
  ring_commands["r_elect"] = CmdType::ElectLeader;
  ring_commands["r_lead"] = CmdType::FindLeader;
}

int getNextId(int id, std::vector<int> active_list) {

  int next_id;

  std::cout << "\nI will search for the next in: \n";

  for (int j = 0; j < active_list.size(); j++)
    std::cout << active_list[j] << "\n";

  if (active_list.size() == 1 && id == active_list[0]) {
    next_id = -1;
  } else {
    auto it = std::find(active_list.begin(), active_list.end(), id);
    int id_position = it - active_list.begin();
    // id changes in position

    if (id_position + 1 == active_list.size())
      next_id = active_list[0];
    else
      next_id = active_list[id_position + 1];
  }

  return next_id;
}

CmdType ringIter(int id, std::vector<int> recv_list, CmdType type) {

  CmdType new_type = CmdType::Error;

  std::cout << "im in ringIter\n";

  if (type == CmdType::NormalRing) {
    std::cout << "returning normal ring...\n";
    new_type = type;
  }

  else if (type == CmdType::FindLeader) {
    if (recv_list[0] == id) {
      std::cout << "returning normal ring...\n";
      new_type = CmdType::NormalRing;
    } else {
      std::cout << "returning find leader...\n";
      new_type = type;
    }
  }

  else if (type == CmdType::ElectLeader) {
    if (recv_list[0] == id) {
      std::cout << "returning find leader...\n";
      new_type = CmdType::FindLeader;
    } else {
      std::cout << "returning elect ring...\n";
      new_type = type;
    }
  }

  return new_type;
}

int electPrimary(std::vector<int> active_list) {

  auto it = std::max_element(std::begin(active_list), std::end(active_list));
  return *it;
}
