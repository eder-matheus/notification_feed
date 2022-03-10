#include "ui.h"
#include <fstream>
#include <iostream>
#include <string>

Ui::Ui(FileType use) {

  ui_use = use;

  switch (ui_use) {
  case FileType::Intro:
    ascii_image.open("ui_files/intro_ascii.txt", std::ios::in);
    break;
  case FileType::Exit:
    ascii_image.open("ui_files/exit_ascii.txt", std::ios::in);
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

void Ui::textBlock(UiType label, std::string message, std::string sender = "0") {

  std::cout << "### ";
  switch (label) {
  case UiType::Message:
    std::cout << "A NOTIFICATION FROM:\n ** " << sender << std::endl;
    std::cout << "- " << message << std::endl;
    break;
  case UiType::Warn:
    std::cout << "WARNING\n";
    std::cout << message << std::endl;
    break;
  case UiType::Error:
    std::cout << "ERROR\n";
    std::cout << message << std::endl;
    break;
  case UiType::Success:
    std::cout << "SUCCESS\n";
    std::cout << message << std::endl;
    break;
  default:
    break;
  }
  std::cout << "###" < std::endl;
}

void Ui::asciiArt() {

  std::string line;
  while (getline(ascii_image, line))
    std::cout << line << std::endl;
}

void Ui::loadingInfo() {}
