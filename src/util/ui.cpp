#include "ui.h"
#include <fstream>
#include <iostream>
#include <string>

Ui::Ui(FileType use) {
  ui_use_ = use;

  switch (ui_use_) {
  case FileType::Intro:
    ascii_image_.open("./ui_files/intro_ascii.txt", std::ios::in);
    break;
  case FileType::Exit:
    ascii_image_.open("./ui_files/exit_ascii.txt", std::ios::in);
    break;
  case FileType::None:
    break;
  default:
    break;
  }

  if (ui_use_ != FileType::None && !ascii_image_.is_open())
    std::cout << "FAILED TO OPEN UI FILE\n";
}
// destructor should close file

void Ui::print(UiType label, const std::string &message,
               const std::string &sender, unsigned long int timestamp) {
  std::cout << "\n";

  switch (label) {
  case UiType::Message:
    std::cout << "[MESSAGE]" << sender << " says: " << message << "("
              << timestamp << ")\n";
    break;
  case UiType::Warn:
    std::cout << "[WARN] " << message << "\n";
    break;
  case UiType::Error:
    std::cout << "[ERROR] " << message << "\n";
    break;
  case UiType::Success:
    std::cout << "[SUCCESS] " << message << "\n";
    break;
  case UiType::Info:
    std::cout << "[INFO] " << message << "\n";
    break;
  default:
    break;
  }
  std::cout << "\n";
}

void Ui::asciiArt() {
  std::string line;
  while (getline(ascii_image_, line))
    std::cout << line << "\n";
}

void Ui::loadingInfo() {}
