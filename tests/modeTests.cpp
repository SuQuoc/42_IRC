#include "TestServer.hpp"

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

void TestServer::tTest(const std::string &mode)
{
    TestServer serv;
    Client *client;
    Channel *channel;


    client = serv.createUserAndChannelRunMode("#Duskwood", "Legolas", "#Duskwood +" + mode, 5);
    channel = serv.getChannel("#Duskwood");
    if(channel->getRestrictTopic() == false)
        return (fail(mode + " should be true 01"));        // should send 324

    serv.runMode(client, "#Duskwood -" + mode);
    if(channel->getRestrictTopic() == true)
        return (fail(mode + " should be false 02"));

    serv.runMode(client, "#Duskwood -" + mode);                   // should trigger 0
    if(channel->getRestrictTopic() == true)
        return (fail(mode + " should be false 03"));

    serv.runMode(client, "#Duskwood -" + mode + " asd asdas3e32 qwsd32er23r " + mode);              // test this line!!!!!??? // should send 324
    if(channel->getRestrictTopic() == false)
        return (fail(mode + " should be true 04"));

    serv.runMode(client, "#Duskwood +" + mode);                   // should return 0
    if(channel->getRestrictTopic() == false)
        return (fail(mode + " should be true 05"));

    // should rm opaerator Legolas and send ERR_NOPRIVILEGES 481
    serv.runMode(client, "#Duskwood -o" + mode + " Legolas"); 
    if(channel->isOperator(client) == true)
        return (fail("client Legolas is still operator after -o Legolas 06"));
    if(channel->getRestrictTopic() == false)
        return (fail(mode + "should be true 07"));
    ok();
}

void TestServer::iTest(const std::string &mode)
{
    TestServer  serv;
    Channel     *ch;
    Client      *client;


    client = serv.createUserAndChannelRunMode("#Duskwood", "Legolas", "#Duskwood +" + mode, 5);
    ch = serv.getChannel("#Duskwood");
    if(ch->getInviteOnly() == false)
        return (fail(mode + " should be true 01"));        // should send 324

    serv.runMode(client, "#Duskwood -" + mode);
    if(ch->getInviteOnly() == true)
        return (fail(mode + " should be false 02"));

    serv.runMode(client, "#Duskwood -" + mode);                   // should trigger 0
    if(ch->getInviteOnly() == true)
        return (fail(mode + " should be false 03"));

    serv.runMode(client, "#Duskwood -" + mode + " asd asdas3e32 qwsd32er23r " + mode);              // test this line!!!!!??? // should send 324
    if(ch->getInviteOnly() == false)
        return (fail(mode + " should be true 04"));

    serv.runMode(client, "#Duskwood +" + mode);                   // should return 0
    if(ch->getInviteOnly() == false)
        return (fail(mode + " should be true 05"));

    // should rm opaerator Legolas and send ERR_NOPRIVILEGES 481
    serv.runMode(client, "#Duskwood -o" + mode + " Legolas"); 
    if(ch->isOperator(client) == true)
        return (fail("client Legolas is still operator after -o Legolas 06"));
    if(ch->getInviteOnly() == false)
        return (fail(mode + "should be true 07"));
    ok();
}

void TestServer::lTest()
{
    /* crateUserAndChannelRunMode(channelname, username, line, fd); */
    ok();
}

void TestServer::mode_tests()
{
    std::cout << "\033[1;33m---MODE TESTS---\033[0m" << std::endl;

    std::cout << "basic test: ";
    basicTest();
    std::cout << "restrict topic test: ";
    tTest("t");
    std::cout << "invite only test: ";
    iTest("i");
    std::cout << "channel limit test: ";
    lTest();
    std::cout << std::endl;
}
