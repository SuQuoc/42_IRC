
#include "TestServer.hpp"

void	TestServer::wrongOperHost()
{
	std::cout << "wrongOperHost: " << std::endl;
	
	TestServer 	serv("Testserver", "password");
	serv.setOperatorHost("host");
	serv.setOperatorPW("pw");

	serv.makeUserJoinChannel("#channel", "nickT", 5); //using it to create a client

	std::stringstream ss("wrong_host pw");
	serv.OPER(serv.getClient(5), ss);
	if (serv.getClient(5)->isServerOp())
		return (fail("client should not be operator, wrong host"));


	std::stringstream ss2("passw");
	serv.OPER(serv.getClient(5), ss2);
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

	std::stringstream ss("host wrong_pw");
	serv.OPER(serv.getClient(5), ss);
	if (serv.getClient(5)->isServerOp())
		return (fail("client should not be operator, wrong pw"));

	std::stringstream ss2("host");
	serv.OPER(serv.getClient(5), ss2);
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

	std::stringstream ss("OpHost OpPass");
	serv.OPER(serv.getClient(5), ss);
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

	std::stringstream ss("host pw");
	serv.OPER(serv.getClient(5), ss);
	if (serv.getClient(5)->isServerOp() == false)
		return (fail("client should be operator"));
	
	
	serv.makeUserJoinChannel("#channel2", "nickT2", 6); //using it to create a client

	std::stringstream ss2("host pw");
	serv.OPER(serv.getClient(6), ss2);
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