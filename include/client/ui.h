#include <fstream>
#include <string>

enum class UiType { Message, Warn, Error, Success };

enum class FileType { None, Intro, Exit };

class Ui {
private:
  std::fstream ascii_image;
  FileType ui_use;

public:
  Ui(FileType);
  void textBlock(UiType, std::string, std::string = "0", unsigned long int timestamp = -1);
  void asciiArt();
  void loadingInfo();
};
