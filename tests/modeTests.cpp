#include "TestServer.hpp"

Client* TestServer::crateUserAndChannelRunMode(const std::string &channelname, const std::string &username, const std::string &line, int fd)
{
    Client *client;

    makeUserJoinChannel(channelname, username, fd);
    client = getClient(username);
    runMode(client, line);
    return (client);
}

void TestServer::runMode(Client *client, const std::string &line)
{
    std::stringstream stream(line);
    MODE(client, stream);
}

void TestServer::basicTest()
{
    TestServer serv;
    Client *client;
    Channel *channel;
    std::string name = "Arwen";
    std::stringstream stream("#Bruchtal +i-i+i-i-i -o Arwen +o Arwen -o Arwen");

    serv.makeUserJoinChannel("#Bruchtal", name, 5);
    client = serv.getClient("Arwen");
    if(client == NULL)
        std::cout << "Client is Null" << std::endl;
    channel = serv.getChannel("#Bruchtal");
    serv.MODE(client, stream);
    if(channel->getInviteOnly() == true)
        fail("invite should be false");
    if(channel->isOperator(client) == true)
        fail("is sill an operartor");
    ok();
}

void TestServer::iTests()
{
    TestServer serv;
    Client *client;
    Channel *channel;

    client = serv.crateUserAndChannelRunMode("#Duskwood", "Legolas", "#Duskwood +i", 5);
    channel = serv.getChannel("#Duskwood");
    if(channel->getInviteOnly() == false)
        return (fail("invite only should be true"));        // should send 324

    serv.runMode(client, "#Duskwood -i");
    if(channel->getInviteOnly() == true)
        return (fail("invite only should be false"));

    serv.runMode(client, "#Duskwood -i");                   // should trigger 0
    if(channel->getInviteOnly() == true)
        return (fail("invite only should be false"));

    serv.runMode(client, "#Duskwood -i asd asdas3e32 qwsd32er23r i");              // test this line!!!!!??? // should send 324
    if(channel->getInviteOnly() == false)
        return (fail("invite only should be true"));

    serv.runMode(client, "#Duskwood +i");                   // should return 0
    if(channel->getInviteOnly() == false)
        return (fail("invite only should be true"));

    // should rm opaerator Legolas and send ERR_NOPRIVILEGES 481
    serv.runMode(client, "#Duskwood -oi Legolas"); 
    if(channel->isOperator(client) == true)
        return (fail("client Legolas is still operator after -o Legolas"));
    if(channel->getInviteOnly() == false)
        return (fail("invite only should be true"));
    ok();
}

void TestServer::mode_tests()
{
    std::cout << "\033[1;33m---MODE TESTS---\033[0m" << std::endl;

    std::cout << "basic test: ";
    basicTest();
    /* std::cout << "invite only test: ";
    iTests(); */
    std::cout << std::endl;
}