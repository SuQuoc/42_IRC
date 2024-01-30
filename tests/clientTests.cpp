#include "TestServer.hpp"
#include <cassert>

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

void TestServer::testMaxChannelsInClient() 
{
    std::cout << "joining more than 10 channels: ";
    TestServer serv;

    Client client(5, "172.16.0.1");
    std::vector<Channel> channels;

    // Create 11 channels
    for (int i = 1; i <= 10; i++) {
        std::string channel_name = "#channel" + std::to_string(i);
        channels.push_back(Channel(&client, channel_name));
    }

    // Attempt to join 11 channels
    for (auto& channel : channels) {
        client.joinChannel(&channel);
    }

    // Check that the client is in the first 10 channels
    for (int i = 1; i <= 10; i++) {
        if (client.isInChannel(&channels[i - 1]) == false)
            return(fail("client is not in channel"));
    }

    // Check that the client is not in the 11th channel
    if (client.isInChannel(&(channels[10])) == true)
        return(fail("client should not be in 11th channel"));

    if (client.getChannelCount() != 10)
        fail("client should have 10 channels");
}


void TestServer::testLeavingOverMaxChannels() 
{
    std::cout << "leaving more than 10 channels: ";
    TestServer serv;

    Client client(5, "172.16.0.1");
    std::vector<Channel> channels;

    // Create 11 channels
    for (int i = 1; i <= 10; i++) {
        std::string channel_name = "#channel" + std::to_string(i);
        channels.push_back(Channel(&client, channel_name));
    }

    // Attempt to join 11 channels
    for (auto& channel : channels) {
        client.joinChannel(&channel);
    }

    //----------------------------------------------
    for (int i = 1; i <= 11; i++) {
        client.leaveChannel(&channels[i - 1]);
        if (i == 3 && client.getChannelCount() != 7)
            return(fail("client should have 7 channels"));
    }

    if (client.getChannelCount() != 0)
        return(fail("client should have 0 channels"));
}

void TestServer::client_tests()
{
	std::cout << "\033[1;33m---CLIENT TESTS---\033[0m" << std::endl;

	testClientInitialization();
	ok();
	testSetNickname();
	ok();
	testSetUser();
	ok();
	testChannelOperations();
	ok();
    testMaxChannelsInClient();
    ok();
    testLeavingOverMaxChannels();
    ok();
}