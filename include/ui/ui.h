#include <string>

enum UiType {MSG, WRN, ERR, SUC};
enum FileType {DEFT, INTR, OUTR};

class Ui {
	private:
		FILE image;

	public:
		Ui(FileType);
		void textBlock(UiType, std::string);
		void clientInit();
		void clientExit();
		void loadingInfo();
};
