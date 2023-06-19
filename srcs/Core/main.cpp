#include <iostream>

#include "../Server/Server.hpp"

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		std::cout << "Usage: ./ircserv port password" << std::endl;
		exit(1);
	}

	try {
		Server server(argv[1]);
		server.pollClientEvents();
	}
	catch (const Server::InitFailed &e)
	{
		std::cerr << e.what() << std::endl;
	}
}
