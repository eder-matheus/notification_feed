#include <string>
#include <fstream>


enum UiType {MSG, WRN, ERR, SUC};
enum FileType {NONE, INTRO, EXIT};

class Ui {
	private:
		std::fstream ascii_image;
		FileType ui_use;

	public:
		Ui(FileType);
		void textBlock(UiType, std::string);
		void asciiArt();
		void loadingInfo();
};
