
#include "TestServer.hpp"


void	TestServer::quitWithoutMessage()
{

}

void	TestServer::quitWithMessage()
{
	std::cout << "quitWithMessage: ";
	TestServer 	serv;
	Client		*elena;

	serv.makeUserJoinChannel("#family", "elena", 5);
	serv.makeUserJoinChannel("#family", "sara", 6);
	
	std::stringstream sstream("#alone");
	serv.JOIN(serv.getClient("elena"), sstream);

	elena = serv.getClient("elena");
	std::stringstream	comment("leaving for a while");
	serv.QUIT(elena, comment);
	
	if (serv._channels.size() != 1 || serv.getChannel("#family") == NULL)
		return (fail("there should be only 1 channel, '#family', left"));
	
	if (serv.getChannelIter("#alone") != serv._channels.end())
		return (fail("channel '#alone' should be deleted"));

	if (serv._client_names.size() != 1) // || serv.getClient("elena") != NULL)
		return (fail("client 'elena' should be deleted from client names map"));	

	if (serv._client_fds.size() != 1 || serv.getClient(5) != NULL)
		return (fail("client 'elena' with fd 5 should be deleted from client fd map"));

	if (serv.getClient("sara") == NULL)
		return (fail("client 'sara' should not be deleted from client names map")); 
	if (serv.getClient(6) == NULL)
		return (fail("client 'sara' with fd 6 should not be deleted from client fd map")); 
	ok();
}

void TestServer::quit_tests()
{
	std::cout << "\033[1;33m---QUIT TESTS---\033[0m" << std::endl;

	quitWithMessage();
}