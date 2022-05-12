#include "common.h"
#include <string>
#include <vector>
#include <unordered_map>

std::unordered_map<std::string, CmdType> ring_commands;

void initRingCommands();

std::vector<int> updateTopology(std::vector<int> active_list, int removedId);

int findFirstNeighboor(int id, int last_try, std::vector<int> topology);

int getNextId(int id, std::vector<int> active_list);

CmdType ringIter(int id, std::vector<int> act_list, std::vector<int> recv_list, CmdType type, bool update);

int electPrimary(std::vector<int> active_list);

