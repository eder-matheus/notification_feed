#include "election.h"
#include <string>
#include <vector>

int findFirstNeighboor(int id, int last_try, std::vector<int> topology) {

  int next_id;

  // if its not the first time
  if (last_try != -1)
    id = last_try;

  auto it = find(topology.begin(), topology.end(), id);
  int id_position = it - topology.begin();

  if (topology[i + 1] == id)
    next_id = -2;
  else if (id_position + 1 == topology.size())
    next_id = topology[0];
  else
    next_id = topology[i + 1]

        return next_id;
}

int getNextId(int id, std::vector<int> active_list) {

  int next_id;

  if (active_list.size() == 1 && id == active_list[0]) {
    next_id = -1
  } else {
    auto it = find(active_list.begin(), active_list.end(), id);
    int id_position = it - topology.begin();
    // id changes in position

    if (id_position + 1 == active_list.size())
      next_id = active_list[0] else next_id = active_list[i + 1]
  }

  return next_id
}

CmdType ringIter(std::vector<int> act_list, std::vector<int> recv_list,
                 CmdType type, bool update) {

  if (type == CmdType::NormalRing) {
    if (active_list == recv_list)
      update = false;
    else
      update = true;
    // active_list = recv_list (one or more fell)
    return CmdType::NormalRing
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
}

int electPrimary(std::vector<int> active_list) {

  auto it = max_element(std::begin(active_list), std::end(active_list));
  return *it;
}
