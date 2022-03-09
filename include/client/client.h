#include <string>

class Client {
	private:
		int id;

	public:
		Client();
		static void* commandToServer(void*);
		int validateCommand(std::string);
		static void* dummy(void*);
		void createConnection();

};

