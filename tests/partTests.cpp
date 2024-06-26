#include "TestServer.hpp"

void	TestServer::partEmptySstream()
{
	TestServer 			serv;
	Client				*elena;

	serv.makeUserJoinChannel("#Channel", "Elena", 5);
	elena = serv.getClient("Elena");
	serv.runPart(elena, "");

	if (elena->getChannelCount() != 1)
		return (fail("elena should be in one channel"));
	ok();
}

void	TestServer::partEmptySpacesSstream()
{
	TestServer 			serv;
	Client				*elena;

	serv.makeUserJoinChannel("#Channel", "Elena", 5);
	elena = serv.getClient("Elena");
	serv.runPart(elena, "   ");

	if (elena->getChannelCount() != 1)
		return (fail("elena should be in one channel"));
	ok();
}

void	TestServer::partChannelNotEmpty()
{
	TestServer 	serv;
	Channel		*channel;
	Client		*elena;
	std::string			channel_name("#channel");

	serv.makeUserJoinChannel(channel_name, "Sara", 5);
	serv.makeUserJoinChannel(channel_name, "Elena", 6);

	channel = serv.getChannel(channel_name);
	elena = serv.getClient("Elena");
	serv.runPart(elena, channel_name);

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

	serv.makeUserJoinChannel(channel_name, "Elena", 6);

	channel = serv.getChannel(channel_name);
	elena = serv.getClient("Elena");
	serv.runPart(elena, channel_name);

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
		serv.runPart(serv._client_fds.find(i)->second, channel_name);
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

	serv.makeUserJoinChannel(channel_name, "Sara", 5);
	serv.makeUser("Elena", 6);

	channel = serv.getChannel(channel_name);
	elena = serv.getClient("Elena");
	serv.runPart(elena, channel_name);

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

	serv.makeUser("Elena", 6);

	channel = serv.getChannel(channel_name);
	elena = serv.getClient("Elena");
	serv.runPart(elena, channel_name);

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
		channels += "#" + std::to_string(i) + ",";
		serv.runJoin(elena, "#" + std::to_string(i));
	}
	if (elena->getChannelCount() != LIST_LIMIT)
		return (fail("elena should be in LIST_LIMIT(10) channels"));
	serv.runPart(elena, channels);
	
	if (elena->getChannelCount() != 0)
		return (fail("elena should have parted from all channels"));
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
		channels += "#" + std::to_string(i) + ",";
		serv.runJoin(elena, "#" + std::to_string(i));
	}
	if (elena->getChannelCount() != LIST_LIMIT)
		return (fail("elena should be in LIST_LIMIT(10) channels"));
	serv.runPart(elena, channels);
	
	if (elena->getChannelCount() != 0)
		return (fail("elena should have parted from all channels"));
	ok();
}

void TestServer::part_tests()
{
	std::cout << "\033[1;33m---PART TESTS---\033[0m" << std::endl;

	std::cout << "empty sstream: ";
	partEmptySstream();
	std::cout << "empty sstream with spaces: ";
	partEmptySpacesSstream();
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
	std::cout << "part over LIST_LIMIT: ";
	partOverListLimit();

	std::cout << std::endl;
}