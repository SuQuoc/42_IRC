
#include "TestServer.hpp"


void TestServer::correctPW()
{
	std::cout << "correct password: ";

	TestServer 	serv("Testserver", "password");
	serv.addNewClientToFdMap(5, "127.0.0.0");
	
	std::string	password("password");
	std::stringstream	sstream(password);

	Client *client = serv.getClient(5);
	serv.PASS(client, sstream);

	if (client == NULL) //set it to NULL after deletion in program
		return (fail("client shouldn't be NULL"));
	if (client->isAuthenticated() == false)
		return (fail("password is correct, client should be authenticated"));
	ok();
}

void TestServer::incorrectPW()
{
	std::cout << "incorrect password: ";

	TestServer 	serv("Testserver", "password");
	serv.makeUserJoinChannel("#family", "elena", 5);
	
	std::string	password("wrong_password");
	std::stringstream	sstream(password);

	Client *client = serv.getClient(5);
	serv.PASS(client, sstream);
	if (serv.getClient(5) != NULL || serv.getClient("elena") != NULL) //set it to NULL after deletion in program
		return (fail("client should be deleted"));

	// if (client->isAuthenticated()) //doenst work because client is deleted
		// return (fail("password was incorrect, client should NOT be authenticated"));
	ok();
}

void TestServer::availableNick()
{
	std::cout << "available nickname: ";
	TestServer 	serv("Testserver", "password");
	serv.addNewClientToFdMap(5, "127.0.0.0");
	serv.addNewClientToFdMap(6, "127.0.0.1");
	
	std::stringstream	ss_pw("password");
	std::stringstream	ss_pw2("password");
	
	Client *client = serv.getClient(5);
	Client *client2 = serv.getClient(6);
	serv.PASS(client, ss_pw);
	serv.PASS(client2, ss_pw2);

	std::stringstream	ss_nick1("Ferdinant");
	serv.NICK(client, ss_nick1);
	if (client->getNickname() != "Ferdinant")
		return(fail("client should be available"));
	
	std::stringstream	ss_nick2("Nick2");
	serv.NICK(client2, ss_nick2);
	if (client2->getNickname() != "Nick2")
		return(fail("client should be available"));
	ok();
}

void TestServer::unavailableNick()
{
	std::cout << "unavailable nickname: ";
	TestServer 	serv("Testserver", "password");
	serv.addNewClientToFdMap(5, "127.0.0.0");
	serv.addNewClientToFdMap(6, "127.0.0.1");
	
	std::stringstream	ss_pw1("password");
	std::stringstream	ss_pw2("password");

	Client *client = serv.getClient(5);
	Client *client2 = serv.getClient(6);
	serv.PASS(client, ss_pw1);
	serv.PASS(client2, ss_pw2);

	std::stringstream	ss_nick1("Ferdinant");
	serv.NICK(client, ss_nick1);
	
	std::stringstream	ss_nick2("Ferdinant");
	serv.NICK(client2, ss_nick2);
	if (client2->getNickname() == "Ferdinant")
		return(fail("nickname should not be available"));
	ok();
}

void TestServer::wrongNick()
{
	std::cout << "wrong nickname: ";
	TestServer 	serv("Testserver", "password");
	serv.addNewClientToFdMap(5, "127.0.0.0");
	
	Client *client = serv.getClient(5);
	std::stringstream	ss_pw1("password");
	serv.PASS(client, ss_pw1);
	
	std::vector<std::string> wrong_nicknames = 
	{
		"", " ", "	", "#nick", "nick#", "nick@", "longerthan9chars", 
		"nick*", "nick,", "nick!", "nick?", "nick@", "nick$", "nick:", "nick.", 
    	"nick#", "nick123456789", "longerthan9chars", "nick123456",
	};

	for (const std::string& nickname : wrong_nicknames) 
	{
		std::stringstream	ss_nick(nickname);
		serv.NICK(client, ss_nick);
		if (client->getNickname().empty() == false)
			return(fail("nickname should be wrong"));
	}
	ok();
}

void	TestServer::registerTwoClients()
{
	TestServer 	serv("Testserver", "password");
	serv.addNewClientToFdMap(5, "127.0.0.0");
	serv.addNewClientToFdMap(6, "127.0.0.1");

	std::stringstream	ss_pw("password");
	std::stringstream	ss_pw2("password");

	Client *client = serv.getClient(5);
	Client *client2 = serv.getClient(6);
	serv.PASS(client, ss_pw);
	serv.PASS(client2, ss_pw2);

	std::stringstream	ss_nick1("Ferdinant");
	serv.NICK(client, ss_nick1);
	
	std::stringstream	ss_nick2("Nick2");
	serv.NICK(client2, ss_nick2);

	//-------------------------------------
	std::cout << "register 2 clients: ";
	std::stringstream	ss_user1("username hostname servername :real name");
	serv.USER(client, ss_user1);

	std::stringstream	ss_user2("username hostname servername :real name");
	serv.USER(client2, ss_user2);
	
	if (serv.getClient("Ferdinant") == NULL)
		return(fail("client 'Ferdinant'should be available")); 
	if (serv.getClient("Nick2") == NULL)
		return(fail("client 'Nick2'should be available"));
	if (client->isRegistered() == false)
		return(fail("client 'Ferdinant' should be registered"));
	if (client2->isRegistered() == false)
		return(fail("client 'Nick2' should be registered"));
	ok();
}




void	TestServer::registration_tests()
{
	std::cout << "\033[1;33m---PASS-NICK-USER TESTS---\033[0m" << std::endl;

	correctPW();
	incorrectPW();
	availableNick();
	unavailableNick();
	wrongNick();
	registerTwoClients();

	std::cout << std::endl;
}