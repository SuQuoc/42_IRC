#include "TestServer.hpp"

void	TestServer::kickEmptySstream()
{
	TestServer 			serv;
	Client				*elena;

	serv.makeUserJoinChannel("#Channel", "Elena", 5);
	elena = serv.getClient("Elena");
	serv.runKick(elena, "");

	if (elena->getChannelCount() != 1)
		return (fail("elena should be in one channel"));
	ok();
}

void	TestServer::kickEmptySpacesSstream()
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

void	TestServer::kickChannelMember()
{
	TestServer	serv;
	Channel		*channel;
	Client		*elena;
	Client		*sara;
	std::string			channel_name = "#channel";

	serv.makeUserJoinChannel(channel_name, "elena", 5);
	serv.makeUserJoinChannel(channel_name, "sara", 6);
	channel = serv.getChannel(channel_name);
	elena = serv.getClient("elena");
	sara = serv.getClient("sara");
	serv.runKick(elena, "#channel sara");

	if (channel->isInChannel(sara) == true)
		return (fail("sara shouldn't be in client-vector"));
	if (sara->isInChannel(channel) == true)
		return (fail("channel shouldn't be in channel-vector"));
	if (channel->isInChannel(elena) == false)
		return (fail("elena should be in client-vector"));
	if (elena->isInChannel(channel) == false)
		return (fail("channel should be in channel-vector"));
	ok();
}

void	TestServer::unauthorizedKick()
{
	TestServer	serv;
	Channel		*channel;
	Client		*elena;
	Client		*sara;
	std::string			channel_name = "#channel";

	serv.makeUserJoinChannel(channel_name, "elena", 5);
	serv.makeUserJoinChannel(channel_name, "sara", 6);
	channel = serv.getChannel(channel_name);
	elena = serv.getClient("elena");
	sara = serv.getClient("sara");
	serv.runKick(sara, "#channel elena");

	if (channel->isInChannel(sara) == false)
		return (fail("sara should be in client-vector"));
	if (sara->isInChannel(channel) == false)
		return (fail("channel should be in channel-vector"));
	if (channel->isInChannel(elena) == false)
		return (fail("elena should be in client-vector"));
	if (elena->isInChannel(channel) == false)
		return (fail("channel should be in channel-vector"));
	ok();
}

void	TestServer::kickSelfAndOnlyMember()
{
	TestServer	serv;
	Channel		*channel;
	Client		*elena;
	std::string			channel_name = "#channel";

	serv.makeUserJoinChannel(channel_name, "elena", 5);
	elena = serv.getClient("elena");
	serv.runKick(elena, "#channel elena");
	channel = serv.getChannel(channel_name);

	if (channel != NULL)
		return (fail("channel should have been deleted"));
	if (elena->isInChannel(channel) == true)
		return (fail("channel shouldn't be in client-vector"));
	ok();
}

void	TestServer::userToKickNotInChannel()
{
	TestServer	serv;
	Channel		*channel;
	Client		*elena;
	std::string			channel_name = "#channel";

	serv.makeUserJoinChannel(channel_name, "elena", 5);
	serv.makeUser("sara", 6);
	channel = serv.getChannel(channel_name);
	elena = serv.getClient("elena");
	serv.runKick(elena, "#channel sara");

	if (channel->isInChannel(elena) == false)
		return (fail("elena should be in client-vector"));
	if (elena->isInChannel(channel) == false)
		return (fail("channel should be in channel-vector"));
	ok();
}

void	TestServer::userToKickNotInMap()
{
	TestServer	serv;
	Channel		*channel;
	Client		*elena;
	std::string			channel_name = "#channel";

	serv.makeUserJoinChannel(channel_name, "elena", 5);
	channel = serv.getChannel(channel_name);
	elena = serv.getClient("elena");
	serv.runKick(elena, "#channel sara");

	if (channel->isInChannel(elena) == false)
		return (fail("elena should be in client-vector"));
	if (elena->isInChannel(channel) == false)
		return (fail("channel should be in channel-vector"));
	ok();
}

void	TestServer::channelNotInMap()
{
	TestServer	serv;
	Channel		*channel;
	Client		*elena;

	serv.makeUser("elena", 5);
	channel = serv.getChannel("#channel");
	elena = serv.getClient("elena");
	serv.runKick(elena, "#channel sara");

	if (channel != NULL)
		return (fail("channel shouldn't exist"));
	if (elena->isInChannel(channel) == true)
		return (fail("channel shouldn't be in channel-vector"));
	ok();
}

void	TestServer::senderNotInChannel()
{
	TestServer	serv;
	Channel		*channel;
	Client		*elena;
	Client		*sara;
	std::string			channel_name = "#channel";

	serv.makeUserJoinChannel(channel_name, "elena", 5);
	serv.makeUser("sara", 6);
	channel = serv.getChannel(channel_name);
	elena = serv.getClient("elena");
	sara = serv.getClient("sara");
	serv.runKick(sara, "#channel elena");

	if (channel->isInChannel(elena) == false)
		return (fail("elena should be in client-vector"));
	if (elena->isInChannel(channel) == false)
		return (fail("channel should be in channel-vector"));
	ok();
}

void	TestServer::kick_tests()
{
	std::cout << "\033[1;33m---KICK TESTS---\033[0m" << std::endl;

	std::cout << "empty sstream: ";
	kickEmptySstream();
	std::cout << "empty sstream with spaces: ";
	kickEmptySpacesSstream();
	std::cout << "kick channel member: ";
	kickChannelMember();
	std::cout << "unauthorized kick: ";
	unauthorizedKick();
	std::cout << "kick self and only member: ";
	kickSelfAndOnlyMember();
	std::cout << "user to kick not in channel: ";
	userToKickNotInChannel();
	std::cout << "user to kick doesn't exist: ";
	userToKickNotInMap();
	std::cout << "channel doesn't exist: ";
	channelNotInMap();
	std::cout << "sender not in channel: ";
	senderNotInChannel();

	std::cout << std::endl;
}