
#include "TestServer.hpp"

void	TestServer::wrongOperHost()
{
	std::cout << "wrongOperHost: " << std::endl;
	
	TestServer 	serv("Testserver", "password");
	serv.setOperatorHost("host");
	serv.setOperatorPW("pw");

	serv.makeUserJoinChannel("#channel", "nickT", 5); //using it to create a client

	serv.runOper(serv.getClient(5), "wrong_host pw");
	if (serv.getClient(5)->isServerOp())
		return (fail("client should not be operator, wrong host"));


	serv.runOper(serv.getClient(5), "passw");
	if (serv.getClient(5)->isServerOp())
		return (fail("client should not be operator, 'empty' host"));

	
	ok();
}


void	TestServer::wrongOperPW()
{
	std::cout << "wrongOperPW: " << std::endl;

	TestServer 	serv("Testserver", "password");
	serv.setOperatorHost("host");
	serv.setOperatorPW("pw");

	serv.makeUserJoinChannel("#channel", "nickT", 5); //using it to create a client

	serv.runOper(serv.getClient(5), "host wrong_pw");
	if (serv.getClient(5)->isServerOp())
		return (fail("client should not be operator, wrong pw"));

	serv.runOper(serv.getClient(5), "host");
	if (serv.getClient(5)->isServerOp())
		return (fail("client should not be operator, 'empty' password"));
	ok();
}


void	TestServer::correctOper()
{
	// std::cout << "correctOper: " << std::endl;
// 
	// TestServer 	serv("Testserver", "password");
	// serv.setOperatorHost("host");
	// serv.setOperatorPW("pw");
// 
	// serv.makeUserJoinChannel("#channel", "nickT", 5); //using it to create a client
// 
	// std::stringstream ss("host pw");
	// serv.OPER(serv.getClient(5), ss);
	// if (serv.getClient(5)->isServerOp() == false)
		// return (fail("client should be operator"));
	// ok();
}

void	TestServer::correctOperDefault()
{
	std::cout << "correctOperDefault: " << std::endl;
	
	TestServer 	serv("Testserver", "password");
	serv.makeUserJoinChannel("#channel", "nickT", 5); //using it to create a client

	serv.runOper(serv.getClient(5), "OpHost OpPass");
	if (serv.getClient(5)->isServerOp() == false)
		return (fail("client should be operator"));
	ok();
}

void TestServer::twoCorrectOper()
{
	std::cout << "correctOper: " << std::endl;

	TestServer 	serv("Testserver", "password");
	serv.setOperatorHost("host");
	serv.setOperatorPW("pw");

	serv.makeUserJoinChannel("#channel", "nickT", 5); //using it to create a client

	serv.runOper(serv.getClient(5), "host pw");
	if (serv.getClient(5)->isServerOp() == false)
		return (fail("client should be operator"));
	
	
	serv.makeUserJoinChannel("#channel2", "nickT2", 6); //using it to create a client

	serv.runOper(serv.getClient(6), "host pw");
	if (serv.getClient(6)->isServerOp() == false)
		return (fail("client should be operator"));
	ok();
}

//can this even be tested? i need to connect to the server to test it
void TestServer::oper_tests()
{
	std::cout << "\033[1;33m---OPER TESTS---\033[0m" << std::endl;

	// wrongOperHost();
	// wrongOperPW();
	// correctOper();
	// twoCorrectOper(); //change, --> test if they can KILL each other
	// correctOperDefault();
}