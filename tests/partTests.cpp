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

	channel = serv.getChannel(channel_name);
	elena = serv.getClient("Elena");
	serv.PART(elena, sstream);

	if (elena == NULL)
		return (fail("elena shouldn't be NULL"));
	if (serv.getChannelIter(channel_name) == serv._channels.end())
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

	channel = serv.getChannel(channel_name);
	elena = serv.getClient("Elena");
	serv.PART(elena, sstream);

	if (elena == NULL)
		return (fail("elena shouldn't be NULL"));
	if (serv.getChannelIter(channel_name) != serv._channels.end())
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
	std::string				channel_name = "#channel";

	for (int i = 0; i < MAX_CLIENTS + 3; i++)
		serv.makeUserJoinChannel(channel_name, std::to_string(i), 5 + i);
	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		std::stringstream	sstream(channel_name);
		serv.PART(serv._client_fds.find(i)->second, sstream);
	}
	
	if (serv.getChannelIter(channel_name) != serv._channels.end())
		return (fail("channel should be deleted"));
}

void	TestServer::userNotInChannel()
{
	TestServer 	serv;
	Channel		*channel;
	Client		*elena;
	std::string			channel_name("#channel");
	std::stringstream	sstream(channel_name);

	serv.makeUserJoinChannel(channel_name, "Sara", 5);
	serv.makeUser("Elena", 6);

	channel = serv.getChannel(channel_name);
	elena = serv.getClient("Elena");
	serv.PART(elena, sstream);

	if (elena == NULL)
		return (fail("elena shouldn't be NULL"));
	if (serv.getChannelIter(channel_name) == serv._channels.end())
		return (fail("channel should not be deleted"));
	if (channel->isInChannel(elena) == true)
		return (fail("client wasn't removed from channel-vector"));
	if (elena->isInChannel(channel) == true)
		return (fail("channel wasn't removed from channel-vector in client"));
	ok();
}

void	TestServer::noSuchChannel()
{
	TestServer 	serv;
	Channel		*channel;
	Client		*elena;
	std::string			channel_name("#channel");
	std::stringstream	sstream(channel_name);

	serv.makeUser("Elena", 6);

	channel = serv.getChannel(channel_name);
	elena = serv.getClient("Elena");
	serv.PART(elena, sstream);

	if (elena == NULL)
		return (fail("elena shouldn't be NULL"));
	if (elena->isInChannel(channel) == true)
		return (fail("channel shouldn't be in channel-vector in client"));
	ok();
}

void	TestServer::partMultipleChannels()
{
	TestServer 		serv;
	Client			*elena;
	std::string		channels;

	serv.makeUser("Elena", 5);
	elena = serv.getClient("Elena");
	for (int i = 0; i < LIST_LIMIT; i++)
	{
		std::stringstream	sstream("#" + std::to_string(i));
		channels += "#" + std::to_string(i) + ",";
		serv.JOIN(elena, sstream);
	}
	if (elena->getChannelCount() != LIST_LIMIT)
		return (fail("elena should be in LIST_LIMIT(10) channels"));
	std::stringstream	part_input(channels);
	serv.PART(elena, part_input);
	
	if (elena->getChannelCount() != 0)
		return (fail("elena should have parted from all channels"));
	ok();
}

void	TestServer::emptySstream()
{
	TestServer 			serv;
	Client				*elena;
	std::stringstream	sstream("");

	serv.makeUser("Elena", 5);
	elena = serv.getClient("Elena");
	serv.PART(elena, sstream);
	ok();
}

void	TestServer::emptySpacesSstream()
{
	TestServer 			serv;
	Client				*elena;
	std::stringstream	sstream("   ");

	serv.makeUser("Elena", 5);
	elena = serv.getClient("Elena");
	serv.PART(elena, sstream);
	ok();
}

void	TestServer::partOverListLimit()
{
	TestServer 		serv;
	Client			*elena;
	std::string		channels;

	serv.makeUser("Elena", 5);
	elena = serv.getClient("Elena");
	for (int i = 0; i < LIST_LIMIT + 5; i++)
	{
		std::stringstream	sstream("#" + std::to_string(i));
		channels += "#" + std::to_string(i) + ",";
		serv.JOIN(elena, sstream);
	}
	if (elena->getChannelCount() != LIST_LIMIT)
		return (fail("elena should be in LIST_LIMIT(10) channels"));
	std::stringstream	part_input(channels);
	serv.PART(elena, part_input);
	
	if (elena->getChannelCount() != 0)
		return (fail("elena should have parted from all channels"));
	ok();
}

void TestServer::part_tests()
{
	std::cout << "\033[1;33m---PART TESTS---\033[0m" << std::endl;

	std::cout << "part channel not empty: ";
	partChannelNotEmpty();
	std::cout << "part channel empty: ";
	partChannelEmpty();
	std::cout << "adding too many users: ";
	addingTooManyUsers();
	std::cout << "user not in channel: ";
	userNotInChannel();
	std::cout << "no such Channel: ";
	noSuchChannel();
	std::cout << "part multiple channels: ";
	partMultipleChannels();
	std::cout << "empty sstream: ";
	emptySstream();
	std::cout << "empty sstream with spaces: ";
	emptySpacesSstream();
	std::cout << "part over LIST_LIMIT: ";
	partOverListLimit();

	std::cout << std::endl;
}