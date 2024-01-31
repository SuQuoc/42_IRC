#include "TestServer.hpp"

Client*	TestServer::makeUserJoinChannel(const std::string &channel_name, const std::string &client_name, int client_fd)
{
	std::stringstream	sstream(channel_name);
	Client*				client;
	
	client = makeUser(client_name, client_fd);
	JOIN(client, sstream);
	return (client);
}

Client*	TestServer::makeUser(const std::string client_name, int client_fd)
{
	std::string			nick_name(client_name);
	Client*				client;

	addNewClientToFdMap(client_fd, "127.0.0.1");
	addClientToNameMap(client_name, client_fd);
	client = getClient(client_fd);
	client->setNickname(client_name);
	client->setUser(nick_name, client_name, client_name, client_name);
	return (client);
}

void	TestServer::fail(const std::string msg) const
{
	std::cout << "\033[0;91m FAIL:\033[0m " << msg << std::endl;
}

void	TestServer::ok() const
{
	std::cout << "\033[0;92m OK\033[0m" << std::endl;
}
