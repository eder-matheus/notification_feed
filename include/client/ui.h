#include <fstream>
#include <string>

enum class UiType { Message, Warn, Error, Success, Info };

enum class FileType { None, Intro, Exit };

class Ui {
private:
  std::fstream ascii_image;
  FileType ui_use;

public:
  Ui(FileType);
  void print(UiType, std::string, std::string = "0", unsigned long int timestamp = 0);
  void asciiArt();
  void loadingInfo();
};
