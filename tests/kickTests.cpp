#include "TestServer.hpp"

void	TestServer::kickEmptySstream()
{
	TestServer 			serv;
	Client				*elena;
	std::stringstream	sstream("");

	serv.makeUserJoinChannel("#Channel", "Elena", 5);
	elena = serv.getClient("Elena");
	serv.KICK(elena, sstream);

	if (elena->getChannelCount() != 1)
		return (fail("elena should be in one channel"));
	ok();
}

void	TestServer::kickEmptySpacesSstream()
{
	TestServer 			serv;
	Client				*elena;
	std::stringstream	sstream("   ");

	serv.makeUserJoinChannel("#Channel", "Elena", 5);
	elena = serv.getClient("Elena");
	serv.PART(elena, sstream);

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
	std::stringstream	sstream("#channel sara");
	std::string			channel_name = "#channel";

	serv.makeUserJoinChannel(channel_name, "elena", 5);
	serv.makeUserJoinChannel(channel_name, "sara", 6);
	channel = serv.getChannel(channel_name);
	elena = serv.getClient("elena");
	sara = serv.getClient("sara");
	serv.KICK(elena, sstream);

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
	std::stringstream	sstream("#channel elena");
	std::string			channel_name = "#channel";

	serv.makeUserJoinChannel(channel_name, "elena", 5);
	serv.makeUserJoinChannel(channel_name, "sara", 6);
	channel = serv.getChannel(channel_name);
	elena = serv.getClient("elena");
	sara = serv.getClient("sara");
	serv.KICK(sara, sstream);

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
	std::stringstream	sstream("#channel elena");
	std::string			channel_name = "#channel";

	serv.makeUserJoinChannel(channel_name, "elena", 5);
	elena = serv.getClient("elena");
	serv.KICK(elena, sstream);
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
	Client		*sara;
	std::stringstream	sstream("#channel sara");
	std::string			channel_name = "#channel";

	serv.makeUserJoinChannel(channel_name, "elena", 5);
	serv.makeUser("sara", 6);
	channel = serv.getChannel(channel_name);
	elena = serv.getClient("elena");
	sara = serv.getClient("sara");
	serv.KICK(elena, sstream);

	if (channel->isInChannel(sara) == false)
		return (fail("sara should be in client-vector"));
	if (sara->isInChannel(channel) == false)
		return (fail("channel should be in channel-vector"));
	if (channel->isInChannel(elena) == false)
		return (fail("elena should be in client-vector"));
	if (elena->isInChannel(channel) == false)
		return (fail("channel should be in channel-vector"));
	ok();hannel()
//void	userToKickNotInMap()
//void	channelNotInMap()

void TestServer::kick_tests()
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

	std::cout << std::endl;
}