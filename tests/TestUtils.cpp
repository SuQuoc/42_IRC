#include "TestServer.hpp"

void	TestServer::makeUserJoinChannel(const std::string &channel_name, const std::string &client_name, int client_fd)
{
	std::stringstream	sstream(channel_name);
	std::string			nick_name(client_name);
	
	addNewClientToFdMap(client_fd, "127.0.0.1");
	addClientToNameMap(client_name, client_fd);
	getClient(client_fd)->setNickname(client_name);
	getClient(client_fd)->setUser(nick_name, client_name, client_name, client_name);
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

int	TestServer::runJoin(Client *client, const std::string& msg)
{
	std::stringstream	sstream(msg);
	return (JOIN(client, sstream));
}

int	TestServer::runKick(Client *client, const std::string& msg)
{
	std::stringstream	sstream(msg);
	return (KICK(client, sstream));
}

int	TestServer::runInvite(Client *client, const std::string& msg)
{
	std::stringstream	sstream(msg);
	return (INVITE(client, sstream));
}

void	TestServer::runMode(Client *client, const std::string &line)
{
	std::stringstream	stream(line);
	MODE(client, stream);
}

Client*	TestServer::createUserAndChannelRunMode(const std::string &channelname, const std::string &username, const std::string &line, int fd)
{
	Client	*client;

	makeUserJoinChannel(channelname, username, fd);
	client = getClient(username);
	runMode(client, line);
	return (client);
}
