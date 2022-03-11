//#include "ui.h"
#include "../../include/client/ui.h"
#include <fstream>
#include <iostream>
#include <string>

Ui::Ui(FileType use) {

  ui_use = use;

  switch (ui_use) {
  case FileType::Intro:
    ascii_image.open("src/client/ui_files/intro_ascii.txt", std::ios::in);
    break;
  case FileType::Exit:
    ascii_image.open("src/client/ui_files/exit_ascii.txt", std::ios::in);
    break;
  case FileType::None:
    break;
  default:
    break;
  }

  if (ui_use != FileType::None && !ascii_image.is_open())
    std::cout << "FAILED TO OPEN UI FILE\n";
}
// destructor should close file

void Ui::textBlock(UiType label, std::string message, std::string sender, int timestamp) {

  std::cout << "### ";
  switch (label) {
  case UiType::Message:
    std::cout << "A NOTIFICATION FROM:\n";
    std::cout << "** " << sender << "\n";
    std::cout << " - " << message << "\n";
    std::cout << " - (" << timestamp << ")\n";
    break;
  case UiType::Warn:
    std::cout << "WARNING\n";
    std::cout << message << "\n";
    break;
  case UiType::Error:
    std::cout << "ERROR\n";
    std::cout << message << "\n";
    break;
  case UiType::Success:
    std::cout << "SUCCESS\n";
    std::cout << message << "\n";
    break;
  default:
    break;
  }
  std::cout << "###" << "\n";
}

void Ui::asciiArt() {

  std::string line;
  while (getline(ascii_image, line))
    std::cout << line << "\n";
}

void Ui::loadingInfo() {}
