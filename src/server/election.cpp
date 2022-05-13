#include "election.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

std::unordered_map<std::string, CmdType> ring_commands;

void initRingCommands() {
  ring_commands["r_new"] = CmdType::NewServer;
  ring_commands["r_norm"] = CmdType::NormalRing;
  ring_commands["r_moni"] = CmdType::MonitorNew;
  ring_commands["r_elec"] = CmdType::ElectLeader;
  ring_commands["r_lead"] = CmdType::FindLeader;
}

int findFirstNeighboor(int id, int last_try, std::vector<int> topology) {

  int next_id;
  int i;
  // if its the first time
  if (last_try == -1)
    last_try = id;

  // auto it = std::find(topology.begin(), topology.end(), last_try);
  // int id_position = it - topology.begin();

  for (i = 0; i < topology.size(); i++) {
    if (topology[i] == last_try)
      break;
  }

  int id_position;
  id_position = i;

  if (id_position + 1 == topology.size()) {
    next_id = topology[0];
    if (next_id == id)
      next_id = -2;
  } else if (topology[id_position + 1] == id)
    next_id = -2;
  else
    next_id = topology[id_position + 1];

  return next_id;
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

CmdType ringIter(int id, std::vector<int> act_list, std::vector<int> recv_list,
                 CmdType type, bool &update) {

  if (type == CmdType::NormalRing) {
    if (act_list == recv_list)
      update = false;
    else
      update = true;
    // active_list = recv_list (one or more fell)
    return CmdType::NormalRing;
  }
  if (type == CmdType::NewServer) {
    if (recv_list.size() == 1)
      return CmdType::MonitorNew;
    else if (recv_list[0] == id)
      return CmdType::NormalRing;
    else
      return CmdType::NewServer;
  }
  if (type == CmdType::ElectLeader) {
    if (recv_list[0] == id)
      return CmdType::FindLeader;
    else
      return CmdType::ElectLeader;
  }
  if (type == CmdType::FindLeader) {
    if (recv_list[0] == id)
      return CmdType::NormalRing;
    else
      return CmdType::FindLeader;
  }
  return CmdType::Error;
}

int electPrimary(std::vector<int> active_list) {

  auto it = std::max_element(std::begin(active_list), std::end(active_list));
  return *it;
}
