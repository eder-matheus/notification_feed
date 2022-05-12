std::unordered_map<std::string, CmdType> ring_commands;
ring_commands["r_new"] = CmdType::NewServer;
ring_commands["r_norm"] = CmdType::NormalRing;
ring_commands["r_moni"] = CmdType::MonitorNew;
ring_commands["r_elec"] = CmdType::ElectLeader;
ring_commands["r_lead"] = CmdType::FindLeader;

std::vector<int> updateTopology(std::vector<int> active_list, int removedId);

int findFirstNeighboor(int id, int last_try, std::vector<int> topology);

int getNextId(int id, std::vector<int> active_list);

CmdType ringIter(std::vector<int> act_list, std::vector<int> recv_list, CmdType type, bool update);

int electPrimary(std::vector<int> active_list);

