#include <string>
#include <iostream>
#include "../../include/ui/ui.h"


Ui::Ui(FileType) {
	
}

void Ui::textBlock(UiType label, std::string message) {

	switch(label) {
		case MSG:
			std::cout << "MESSAGE\n";
			std::cout << message << std::endl;
			break;
		case WRN:
			std::cout << "WARNING\n";
			std::cout << message <<  std::endl;
			break;
		case ERR:
			std::cout << "ERROR\n";
			std::cout << message << std::endl;
			break;
		case SUC:
			std::cout << "SUCCESS\n";
			std::cout << message << std::endl;
			break;
	}
}

void Ui::clientInit() {
}

void Ui::clientExit() {
}

void Ui::loadingInfo() {
}


