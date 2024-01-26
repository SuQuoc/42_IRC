#include <TestServer.hpp>

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
	if (elena.)
	
	ok();
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