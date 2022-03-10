#include <string>
#include <iostream>
#include <fstream>
#include "ui.h"


Ui::Ui(FileType use) {

	ui_use = use;	

	switch(ui_use) {
		case INTRO:
			ascii_image.open("src/ui/ui_files/ufrgs_logo_ascii.txt", std::ios::in);
			break;
		case EXIT:
			break;
		case NONE:
			break;
		default:
			break;
	}

	if(ui_use != NONE && !ascii_image.is_open())
		std::cout << "FAILED TO OPEN UI FILE\n";
}
//destructor should close file

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
		default:
			break;
	}
}

void Ui::asciiArt() {

	std::string line;
	while(getline(ascii_image, line))
		std::cout << line << std::endl;
}

void Ui::loadingInfo() {
}


