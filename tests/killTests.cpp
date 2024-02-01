
#include "TestServer.hpp"


void TestServer::succesfulKill()
{
	std::cout << "succesfulKILL: ";
	TestServer 	serv("Testserver", "password");
	serv.setOperatorHost("host");
	serv.setOperatorPW("pw");

	serv.makeUserJoinChannel("#family", "elena", 5); //using it to create a client
	serv.makeUserJoinChannel("#family", "sara", 6); //using it to create a client

	serv.runJoin(serv.getClient("elena"), "#alone");

	serv.runOper(serv.getClient("sara"), "host pw");
	//----make someone IRC operator----


	serv.runKill(serv.getClient("sara"), "elena :sry big sis i dont want u here");

	//----test from QUIT--------------
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


void TestServer::notAnIrcOper()
{
	std::cout << "not an irc operator: ";
	TestServer 	serv("Testserver", "password");
	serv.setOperatorHost("host");
	serv.setOperatorPW("pw");

	serv.makeUserJoinChannel("#family", "elena", 5); //using it to create a client
	serv.makeUserJoinChannel("#alone", "sara", 6); //using it to create a client

	serv.runKill(serv.getClient("sara"), "elena :sry big sis i dont want u here");

	if (serv.getClient("sara") == NULL)
		return (fail("client 'sara' should not be deleted from client names map"));

	if (serv.getClient(6) == NULL)
		return (fail("client 'sara' with fd 6 should not be deleted from client fd map"));

	if (serv._channels.size() != 2)
		return (fail("there should be 2 channels, '#family' and '#alone', left")); 
	
	if (serv._client_fds.size() != 2 || serv._client_names.size() != 2)
		return (fail("there should be 2 clients left"));

	ok();
}

//can this even be tested? i need to connect to the server to test it
void TestServer::kill_tests()
{
	std::cout << "\033[1;33m---KILL TESTS---\033[0m" << std::endl;

	// succesfulKill();
	// notAnIrcOper();
}
