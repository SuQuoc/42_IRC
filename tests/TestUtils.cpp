#include "TestServer.hpp"

void	TestServer::makeUserJoinChannel(const std::string channel_name, const std::string client_name, int client_fd)
{
	std::stringstream		sstream(channel_name);

	addNewClientToFdMap(client_fd, "127.0.0.1");
	addClientToNameMap(client_name, client_fd);
	JOIN(_client_fds.find(client_fd)->second, sstream);
}

void	TestServer::makeUser(const std::string client_name, int client_fd)
{
	addNewClientToFdMap(client_fd, "127.0.0.1");
	addClientToNameMap(client_name, client_fd);
}

void	TestServer::fail(const std::string msg) const
{
	std::cout << "\033[0;91m FAIL:\033[0m " << msg << std::endl;
}

void	TestServer::ok() const
{
	std::cout << "\033[0;92m OK\033[0m" << std::endl;
}
