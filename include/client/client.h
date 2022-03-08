#include <string>

class Client {
	private:
		int id;

	public:
		Client();
		void commandToServer();
		int validateCommand(std::string);
};

