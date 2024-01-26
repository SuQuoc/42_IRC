#include "TestServer.hpp"

void	TestServer::partChannelNotEmpty()
{
	TestServer 	serv;
	Channel		*channel;
	Client		*elena;

	serv.makeUserJoinChannel("#channel", "Sara", 5);
	serv.makeUserJoinChannel("#channel", "Elena", 6);

	channel = serv._channels.find("#channel")->second;
	elena = serv._client_names.find("Elena")->second;
	serv.PART(elena, makeSstream("#channel"));

	if (channel->isInChannel(elena) == true)
		return (fail("client wasn't removed from channel-vector"));
	if (elena->isInChannel(channel) == true)
		return (fail("channel wasn't removed from channel-vector in client"));
	if (serv._channels.find("#channel") == serv._channels.end())
		return (fail("channel should not be deleted"));
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
		serv.PART(serv._client_fds.find(i)->second, makeSstream("#channel"));
	
	if (serv._channels.find("#channel") != serv._channels.end())
		return (fail("channel should be deleted"));
}

void TestServer::part_tests()
{
	std::cout << "\033[1;33m---PART TESTS---\033[0m" << std::endl;

	std::cout << "part channel not empty: ";
	partChannelNotEmpty();
	std::cout << "adding too many users: ";
	addingTooManyUsers();

	std::cout << std::endl;
}