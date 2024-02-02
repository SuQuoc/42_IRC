
#include "TestServer.hpp"


void TestServer::correctPW()
{
	std::cout << "correct password: ";

	TestServer 	serv("Testserver", "password");
	serv.addNewClientToFdMap(5, "127.0.0.0");

	Client *client = serv.getClient(5);
	serv.runPass(client, "password");

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

	Client *client = serv.getClient(5);
	serv.runPass(client, "187");
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
	
	Client *client = serv.getClient(5);
	Client *client2 = serv.getClient(6);
	serv.runPass(client, "password");
	serv.runPass(client2, "password");

	serv.runNick(client, "Ferdinant");
	if (client->getNickname() != "Ferdinant")
		return(fail("client should be available"));

	serv.runNick(client2, "Nick2");
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

	Client *client = serv.getClient(5);
	Client *client2 = serv.getClient(6);
	serv.runPass(client, "password");
	serv.runPass(client2, "password");

	serv.runNick(client, "Ferdinant");
	
	serv.runNick(client2, "Ferdinant");
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
	serv.runPass(client, "password");
	
	std::vector<std::string> wrong_nicknames = 
	{
		"", " ", "	", "#nick", "nick#", "nick@", "longerthan9chars", 
		"nick*", "nick,", "nick!", "nick?", "nick@", "nick$", "nick:", "nick.", 
    	"nick#", "nick123456789", "longerthan9chars", "nick123456",
	};

	for (const std::string& nickname : wrong_nicknames) 
	{
		serv.runNick(client, nickname);
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

	Client *client = serv.getClient(5);
	Client *client2 = serv.getClient(6);
	serv.runPass(client, "password");
	serv.runPass(client2, "password");

	serv.runNick(client, "Ferdinant");
	
	serv.runNick(client2, "Nick2");

	//-------------------------------------
	std::cout << "register 2 clients: ";
	serv.runUser(client, "username hostname servername :real name");

	serv.runUser(client2, "username hostname servername :real name");
	
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