#include <fstream>
#include <string>

enum class UiType { Message, Warn, Error, Success, Info };

enum class FileType { None, Intro, Exit };

class Ui {
private:
  std::fstream ascii_image_;
  FileType ui_use_;

public:
  Ui() = default;
  Ui(FileType);
  void print(UiType label, const std::string &message, const std::string &sender = "0",
             unsigned long int timestamp = 0);
  void asciiArt();
  void loadingInfo();
};
