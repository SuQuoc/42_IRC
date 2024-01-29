#include "TestServer.hpp"
#include <cassert>

/* void TestServer::pass_tests()
{
	std::cout << "\033[1;33m---PASS TESTS---\033[0m" << std::endl;

 	TestServer	serv;
    Client		client(1, "1");
    std::stringstream ss("already authenticated");
    int fd = 0;

    // Test case where client is already registered
    client.authenticate();
    serv.PASS(&client, ss, fd);
    
    // Test case where password is correct
    client.deauthenticate();
    ss << "correct_password";
    serv.PASS(&client, ss, fd);

    // Test case where password is incorrect
    ss.clear();
    ss << "incorrect_password";
    serv.PASS(&client, ss, fd);

	std::cout << std::endl;
} */


void testClientInitialization() 
{
    int socketFd = 123;
    std::string ip_addr = "192.168.1.1";
    Client client(socketFd, ip_addr);

    assert(client.getFd() == socketFd);
    assert(client.getNickname() == "");
    assert(client.getUsername() == "");
    assert(client.getHost() == "192.168.1.1");
    assert(client.getPrefix() == "");
    assert(client.getAllChannels().empty());
    assert(!client.isRegistered());
    assert(!client.isAuthenticated());
    assert(!client.isServerOp());
    assert(client.getChannelCount() == 0);
	std::cout << "ClientInitialization: ";
}

void testSetNickname() {
    Client client(456, "10.0.0.1");
    assert(client.setNickname("nick") == 0);
    assert(client.getNickname() == "nick");
	std::cout << "setNickname: ";
}

void testSetUser() {
    Client client(789, "172.16.0.1");
    std::string uname = "user";
    std::string hname = "host";
    std::string sname = "server";
    std::string rname = "real";

    assert(client.setUser(uname, hname, sname, rname) == 0);
    assert(client.getUsername() == uname);
    assert(client.getHost() == "172.16.0.1");
	std::cout << "setUser: ";
    // Add more checks for other user-related information
}

void testChannelOperations() 
{
    Client client(789, "172.16.0.1");
    Channel channel1(&client, "#channel1");

    client.joinChannel(&channel1);
    assert(client.isInChannel(&channel1));
    assert(client.getChannelCount() == 1);

    client.leaveChannel(&channel1);
    assert(!client.isInChannel(&channel1));
    assert(client.getChannelCount() == 0);
	std::cout << "joining and leaving a channel: ";
}

void testMaxChannels() 
{
    Client client(789, "172.16.0.1");
    std::vector<Channel> channels;

    // Create 11 channels
    for (int i = 1; i <= 11; ++i) {
        channels.push_back(Channel(&client, "#channel" + std::to_string(i)));
    }

    // Attempt to join 11 channels
    // for (const auto& channel : channels) {
        // client.joinChannel(&channel);
    // }

    // Check that the client is in the first 10 channels
    for (int i = 1; i <= 10; ++i) {
        assert(client.isInChannel(&channels[i - 1]));
    }

    // Check that the client is not in the 11th channel
    assert(!client.isInChannel(&channels[10]));

    // Check that the client's channel count is 10
    assert(client.getChannelCount() == 10);

    std::cout << "joining more than 10 channels: ";
}


void TestServer::client_tests()
{
	std::cout << "\033[1;33m---CLIENT TESTS---\033[0m" << std::endl;

 	TestServer	serv;
    Client		client(1, "1");

	Channel		channel1(&client, "#channel1");

	testClientInitialization();
	ok();
	testSetNickname();
	ok();
	testSetUser();
	ok();
	testChannelOperations();
	ok();
}