#include "TestServer.hpp"

void	TestServer::partChannelNotEmpty()
{
	TestServer 	serv;
	Channel		*channel;
	Client		*elena;
	std::string			channel_name("#channel");
	std::stringstream	sstream(channel_name);

	serv.makeUserJoinChannel(channel_name, "Sara", 5);
	serv.makeUserJoinChannel(channel_name, "Elena", 6);

	channel = serv._channels.find(channel_name)->second;
	elena = serv._client_names.find("Elena")->second;
	serv.PART(elena, sstream);

	if (elena == NULL)
		return (fail("elena shouldn't be NULL"));
	if (serv._channels.find(channel_name) == serv._channels.end())
		return (fail("channel should not be deleted"));
	if (channel->isInChannel(elena) == true)
		return (fail("client wasn't removed from channel-vector"));
	if (elena->isInChannel(channel) == true)
		return (fail("channel wasn't removed from channel-vector in client"));
	ok();
}

void	TestServer::partChannelEmpty()
{
	TestServer 	serv;
	Channel		*channel;
	Client		*elena;
	std::string			channel_name("#channel");
	std::stringstream	sstream(channel_name);

	serv.makeUserJoinChannel(channel_name, "Elena", 6);

	channel = serv._channels.find(channel_name)->second;
	elena = serv._client_names.find("Elena")->second;
	serv.PART(elena, sstream);

	if (elena == NULL)
		return (fail("elena shouldn't be NULL"));
	if (serv._channels.find(channel_name) != serv._channels.end())
		return (fail("channel should not be deleted"));
	if (elena->isInChannel(channel) == true)
		return (fail("channel wasn't removed from channel-vector in client"));
	ok();
}

//too many users join and all part so channel in channels-map should be deleted
void	TestServer::fromTooManyUsersToChannelEmpty()
{
	TestServer 				serv;
	channel_map_iter_t		channel_it;

	for (int i = 0; i < MAX_CLIENTS + 3; i++)
		serv.makeUserJoinChannel("#channel", std::to_string(i), 5 + i);
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		std::stringstream	sstream("#channel");
		serv.PART(serv._client_fds.find(i)->second, sstream);
	}
	
	if (serv._channels.find("#channel") != serv._channels.end())
		return (fail("channel should be deleted"));
}

/* void	TestServer::userNotInChannel()
{
	TestServer	serv;

} */

//void	TestServer::noSuchChannel();

void TestServer::part_tests()
{
	std::cout << "\033[1;33m---PART TESTS---\033[0m" << std::endl;

	std::cout << "part channel not empty: ";
	partChannelNotEmpty();
	std::cout << "part channel empty: ";
	partChannelEmpty();
	std::cout << "adding too many users: ";
	addingTooManyUsers();

	std::cout << std::endl;
}