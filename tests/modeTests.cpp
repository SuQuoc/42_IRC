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
    TestServer  serv;
    Client*     client;
    Channel*    channel;


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
    TestServer  serv;
    Client*     sylvanas = serv.makeUserJoinChannel("#Silbermond", "Sylvanas", 5);;
    Client*     vonix = serv.makeUser("Vonix", 6);
    Channel*    ch = serv.getChannel("#Silbermond");

    serv.runMode(sylvanas, "#Silbermond +l 144");
    if(ch->getMaxClients() != MAX_CLIENTS)
        return (fail("Should still be MAX_CLIENTS (01)"));

    serv.runMode(sylvanas, "#Silbermond    +l    1         3;[[;];'];]              -l");
    ch->addClient(vonix, "", false);
    if(ch->getMaxClients() != 1)
        return (fail("Should be 1 (02)"));
    if(ch->isInChannel(vonix) == true)
        return (fail("Vonix Should not be in the channel (03)"));

    serv.runMode(sylvanas, "#Silbermond    +l    2         3;[[;];'];]              -dwqrel     ");
    ch->addClient(vonix, "", false);
    if(ch->getMaxClients() != 2)
        return (fail("Should be 2 (04)"));
    if(ch->isInChannel(vonix) == false)
        return (fail("Vonix Should in the channel (05)"));
    
    serv.runMode(sylvanas, "#Silbermond    +l    2         3;[[;];'];]              -dwqrel     ");
    if(ch->getMaxClients() != MAX_CLIENTS)
        return (fail("Should be MAX_CLIENTS (06)"));

    serv.runMode(sylvanas, "#Silbermond    +l    -2         +l 96");
    if(ch->getMaxClients() != 96)
        return (fail("Should be 96 (07)"));
    serv.runMode(sylvanas, "#Silbermond");
    if(ch->getMaxClients() != 96)
        return (fail("Should be 96 (07)"));
    ok();
}

//channel name is set from cChannel *ch
void TestServer::runModeCheckOperator(Client *client, Client *client_target, Channel *ch, std::string run_line, std::string error_msg, bool trigger_error, bool &error)
{
    runMode(client, ch->getName() + " " + run_line);
    if(ch->isOperator(client_target) == trigger_error)
    {
        fail(error_msg);
        error = true;
    }
}

void TestServer::oTest()
{
    TestServer  serv;
    Channel*    ch;
    Client*     sylvanas = serv.makeUserJoinChannel("#Silbermond", "Sylvanas", 5);;
    Client*     vonix = serv.makeUserJoinChannel("#Silbermond", "Vonix", 6);
    bool        error = false;

    ch = serv.getChannel("#Silbermond");
    serv.runModeCheckOperator(sylvanas, vonix, ch, "+o Vonix", "Vonix should be operator 01", false, error);
    serv.runModeCheckOperator(sylvanas, vonix, ch, "-o+o-o Vonix Vonix Vonix", "Vonix should not be operator 02", true, error);
    serv.runModeCheckOperator(sylvanas, vonix, ch, "-o -o Vonix Vonix -Vonix Vonix", "Vonix should not be operator 03", true, error);
    serv.runModeCheckOperator(sylvanas, vonix, ch, "[]23423501=34[]]", "Vonix should not be operator 04", true, error);
    serv.runModeCheckOperator(sylvanas, vonix, ch, "-o -o Vonix Vonix -Vonix Vonix +o +o oooooooooo Vonix", "Vonix should be operator 05", false, error);
    serv.runModeCheckOperator(sylvanas, vonix, ch, "-o+o-o +o Vonix Vonixa", "Vonix should be operator 06", false, error);
    serv.runModeCheckOperator(sylvanas, vonix, ch, "-o-o Sylvanas Vonix", "Vonix should be operator 07", false, error);
    if(ch->isOperator(sylvanas) == true)
        return (fail("Silvanas should not be operator"));
    serv.runModeCheckOperator(sylvanas, vonix, ch, "", "Vonix should be operator 07", false, error);
    if(error == false)
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
    std::cout << "channel change operator test: ";
    oTest();
    std::cout << std::endl;
}
