#include "TestServer.hpp"

Client*	TestServer::makeUserJoinChannel(const std::string &channel_name, const std::string &client_name, int client_fd)
{
	std::stringstream	sstream(channel_name);
	Client*				client;
	
	client = makeUser(client_name, client_fd);
	setSender(client);
	JOIN(sstream);
	return (client);
}

Client*	TestServer::makeUser(const std::string client_name, int client_fd)
{
	std::string			nick_name(client_name);
	Client*				client;

	addNewClientToFdMap(client_fd, "127.0.0.1", 0);
	addClientToNameMap(client_name, client_fd);
	client = getClient(client_fd);
	client->setNickname(client_name);
	client->setUser(nick_name, client_name, client_name, client_name);
	return (client);
}

void	TestServer::fail(const std::string msg) const
{
	std::cout << "\033[0;91m FAIL:\033[0m " << msg << std::endl;
	exit(1);
}

void	TestServer::ok() const
{
	std::cout << "\033[0;92m OK\033[0m" << std::endl;
}

int	TestServer::runJoin(Client *client, const std::string& msg)
{
	std::stringstream	sstream(msg);
	setSender(client);
	return (JOIN(sstream));
}

int	TestServer::runKick(Client *client, const std::string& msg)
{
	std::stringstream	sstream(msg);
	setSender(client);
	return (KICK(sstream));
}

int	TestServer::runInvite(Client *client, const std::string& msg)
{
	std::stringstream	sstream(msg);
	setSender(client);
	return (INVITE(sstream));
}

void	TestServer::runMode(Client *client, const std::string &line)
{
	std::stringstream	stream(line);
	setSender(client);
	MODE(stream);
}

void	TestServer::runPart(Client *client, const std::string &line)
{
	std::stringstream	stream(line);
	setSender(client);
	PART(stream);
}

void	TestServer::runKill(Client *client, const std::string &line)
{
	std::stringstream	stream(line);
	setSender(client);
	KILL(stream);
}

void	TestServer::runOper(Client *client, const std::string &line)
{
	std::stringstream	stream(line);
	setSender(client);
	OPER(stream);
}

void	TestServer::runUser(Client *client, const std::string &line)
{
	std::stringstream	stream(line);
	setSender(client);
	USER(stream);
}

void	TestServer::runNick(Client *client, const std::string &line)
{
	std::stringstream	stream(line);
	setSender(client);
	NICK(stream);
}

void	TestServer::runPass(Client *client, const std::string &line)
{
	std::stringstream	stream(line);
	setSender(client);
	PASS(stream);
}

void TestServer::runQuit(Client *client, const std::string &line)
{
	std::stringstream	stream(line);
	setSender(client);
	QUIT(stream);
}

Client*	TestServer::createUserAndChannelRunMode(const std::string &channelname, const std::string &username, const std::string &line, int fd)
{
	Client	*client;

	makeUserJoinChannel(channelname, username, fd);
	client = getClient(username);
	runMode(client, line);
	return (client);
}
