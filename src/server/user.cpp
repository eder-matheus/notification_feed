//#include "user.h"
#include "../../include/server/user.h"

User::User(std::string name)
        : username_(name),
            sessions_(0)
{
}
