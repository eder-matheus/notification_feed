#include "../../include/client/client.h"
#include "../../include/util/definitions.h"
#include <string>
#include <iostream>


Client::Client() {
}

int Client::validateCommand(std::string input) {

	std::string command = input.substr(0, input.find(' '));
	input.erase(0, input.find(' ') + sizeof(char));
	std::string message = input;

	if(command.compare(CMD_FOLLOW) == 0) {
		std::cout << "follow command\n";
	}
	else if(command.compare(CMD_SEND) == 0) {
		std::cout << "send command\n";
	}
	else {
		std::cout << "invalid command\n";
		return -1;
	}

	return 0;
}

void Client::commandToServer() {
	
	std::string input;

	while(true) {
		std::getline(std::cin, input);
		if(this -> validateCommand(input) < 0)
			std::cout << "TO KILL LOOP";
			//return -1;
		std::cout << "end of loop\n";
	}

}
