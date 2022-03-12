#include "user.h"

User::User(std::string name)
        : username_(name),
            sessions_(0)
{
}
