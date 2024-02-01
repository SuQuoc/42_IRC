#include "TestServer.hpp"

void	TestServer::makingNewChannel()
{
	TestServer 				serv;
	std::vector<Channel*>	chan_vector;
	channel_map_iter_t		channel_it;
	Client					*client;

	serv.makeUserJoinChannel("#channel", "Jan", 5);

	client = serv._client_fds.find(5)->second;
	if (client == NULL)
		return (fail("Client is NULL"));
	channel_it = serv._channels.find("#channel");

	if (channel_it == serv._channels.end())
		return (fail("can't add channel to channel-map"));
	if (!channel_it->second->isInChannel(client))
		return (fail("channel-class doesn't add client to channel-vector"));
	if (!channel_it->second->isOperator(client))
		return (fail("first client isn't operator"));
	if (channel_it->second->size() != 1)
		return (fail("channel vector isn't right size"));

	chan_vector = client->getAllChannels();
	if (chan_vector.size() != 1)
		return (fail("client doesn't have channel"));
	ok();
}

void	TestServer::addingTooManyUsers()
{
	TestServer 				serv;
	channel_map_iter_t		channel_it;

	for (int i = 0; i < MAX_CLIENTS - 1; i++)
		serv.makeUserJoinChannel("#channel", std::to_string(i), 5 + i);

	if (serv._channels.size() != 1)
		return (fail("should be one channel"));
	if (serv._channels.find("#channel")->second->size() != MAX_CLIENTS - 1)
		return (fail("should be MAX_CLIENTS - 1"));

	serv.makeUserJoinChannel("#channel", std::to_string(MAX_CLIENTS), 5 + MAX_CLIENTS);
	if (serv._channels.find("#channel")->second->size() != MAX_CLIENTS)
		return (fail("should be MAX_CLIENTS"));

	serv.makeUserJoinChannel("#channel", std::to_string(MAX_CLIENTS + 1), 5 + MAX_CLIENTS + 1);
	if (serv._channels.find("#channel")->second->size() == MAX_CLIENTS + 1)
		return (fail("over channel limit"));
	ok();
}

void TestServer::addingListOfChannels()
{
	TestServer				serv;
	Client 					*client;
	std::vector<Channel*>	chan_vector;

	serv.makeUserJoinChannel("#ch1,#ch2,#ch3,#ch4,#ch5", "Weex", 5);

	client = serv._client_fds.find(5)->second;
	if (client == NULL)
		return (fail("Client is NULL"));
	chan_vector = client->getAllChannels();

	if (serv._channels.size() != 5)
		return (fail("should be 5 channels"));
	if (chan_vector.size() != 5)
		return (fail("client doesn't have all channels"));
	ok();
}

void TestServer::CheckingChannelNames()
{
	TestServer	serv;
	std::string	channel_names("#ch1,#ch2,#ch3,#ch4,&ch5,ch6");

	serv.makeUserJoinChannel(channel_names + ", #notWorking", "Fio", 5);

	std::string::iterator channel_names_it = channel_names.begin();
	for (channel_map_iter_t channel_it = serv._channels.begin(); channel_it != serv._channels.end(); channel_it++)
	{
		if(channel_it->second->getName().compare(std::string{channel_names_it, channel_names_it + 4}) != 0)
			return (fail("channel names are not the same"));
		channel_names_it += 5;
	}
	if (serv._channels.size() != 5)
		return (fail("should be 5 channels"));
	std::vector<Channel*>	chan_vector = serv._client_fds.find(5)->second->getAllChannels();
	if (chan_vector.size() != 5)
		return (fail("client doesn't have all channels"));
	ok();
}

void TestServer::CheckingAmmountOfChannels(const size_t &expected_total_ch_ammount, const size_t &expected_ch_ammount_in_clients, const int &fd)
{
	Client 					*client;
	std::vector<Channel*>	chan_vector;

	client = _client_fds.find(fd)->second;
	if (client == NULL)
		return (fail("Client is NULL"));
	chan_vector = client->getAllChannels();

	if (_channels.size() != expected_total_ch_ammount)
		return (fail("wrong ammount of channels"));
	if (chan_vector.size() != expected_ch_ammount_in_clients)
		return (fail("client doesn't have all channels"));
}

void TestServer::CheckingWrongListInput()
{
	TestServer	serv;
	int channal_ammount = 2;

	//add 2 channels
	serv.makeUserJoinChannel("     #ch1,#ch2, #ch3, #ch4,#ch5", "Ferdinant", 5);
	serv.CheckingAmmountOfChannels(channal_ammount, 2, 5);

	//add 1 channels
	serv.makeUserJoinChannel("ch1,*ch2,,#ch,ch3, #ch4,#ch5", "Michael", 6);
	channal_ammount++;
	serv.CheckingAmmountOfChannels(channal_ammount, 1, 6);

	//add 0 channels and join #ch1 #ch2
	serv.makeUserJoinChannel("#ch1,#ch2, #ch3,#ch4", "Tanja", 7);
	serv.CheckingAmmountOfChannels(channal_ammount, 2, 7);
	if (serv._channels.find("#ch1")->second->size() != 2)
		return (fail("should be 2"));
	if (serv._channels.find("#ch2")->second->size() != 2)
		return (fail("should be 2"));
	ok();
}

void TestServer::addChannelWithPWandJoin()
{
	Client		*channel_ower;
	Channel		*channel;
	Client		*michi;
	Client		*niki;
	TestServer	serv;
	
	serv.makeUserJoinChannel("#pw", "Fiona", 5);
	channel_ower = serv.getClient("Fiona");

	channel = serv.getChannel("#pw");
	if(channel == NULL)
		return (fail("channel was not created"));
	channel->setPassword("nyancat", '+');
	if(channel->getPassword() != "nyancat")
		return (fail("password was wrong set or empty"));
	
	serv.makeUserJoinChannel("#pw", "Michi", 6);
	michi = serv.getClient("Michi");
	if(!michi)
		return (fail("user was not created"));
	if(channel->isInChannel(michi) == true)
		return (fail("password faild user joined without password"));

	serv.makeUserJoinChannel("#pw 12345", "Herbert", 7);
	michi = serv.getClient("Herbert");
	if(!michi)
		return (fail("user was not created"));
	if(channel->isInChannel(michi) == true)
		return (fail("password faild user joined with wrong password"));

	serv.makeUserJoinChannel("#pw nyancat", "Niki", 8);
	niki = serv.getClient("Niki");
	if(!niki)
		return (fail("user was not created"));
	if(channel->isInChannel(niki) == false)
		return (fail("password was correct but user did not join"));

	channel->setPassword("nyancat3", '-');
	if(channel->getPassword() != "nyancat")
		return (fail("pw was removed with wrong pw"));
	
	channel->setPassword("nyancat", '-');
	if(channel->getPassword() == "nyancat")
		return (fail("pw was not removed"));
	
	serv.makeUserJoinChannel("#pw 12345", "Tim", 9);
	michi = serv.getClient("Tim");
	if(!michi)
		return (fail("user was not created"));
	if(channel->isInChannel(michi) == false)
		return (fail("user did not join"));

	ok();
}

void TestServer::wrongChannelName()
{
	TestServer	serv;

	serv.makeUserJoinChannel("#,~ch1,*ch2,#bell\a,#ch3,: ch4", "Sandi", 5);
	serv.CheckingAmmountOfChannels(2, 2, 5);

	serv.makeUserJoinChannel("ä123,#ch1,:#c,h4", "Niko", 6);
	serv.CheckingAmmountOfChannels(3, 1, 6);

	serv.makeUserJoinChannel("ä123,#ch1,:#c,h4", "Laika", 7);
	serv.CheckingAmmountOfChannels(3, 1, 7);

	ok();
}

void TestServer::tooLongList()
{
	TestServer serv;

	serv.makeUserJoinChannel("#1,#2,#3,#4,#5,#6,#7,#8,#9,#10,#11,#12", "Dany", 5);
	if(serv._channels.size() != LIST_LIMIT)
		return (fail("over LIST_LIMIT in Aserver"));
	if(serv.getClient("Dany")->getAllChannels().size() != LIST_LIMIT)
		return (fail("over LIST_LIMIT in _channles in Client"));
	ok();
}

void	TestServer::createChannelTestCnt(std::string join_input, size_t ch_max_Aserver, size_t ch_max_client)
{
	TestServer serv;

	serv.makeUserJoinChannel(join_input, "Dany", 5);
	if(serv._channels.size() != ch_max_Aserver)
		fail("over LIST_LIMIT in Aserver");
	else if(serv.getClient("Dany")->getAllChannels().size() != ch_max_client)
		fail("over LIST_LIMIT in _channles in Client");
}

void	TestServer::random_input()
{
	createChannelTestCnt("#1,#:2 a", 2, 2);
	createChannelTestCnt(":#123123", 1, 1);
	createChannelTestCnt("#:::::::  2312 2132 1123 12321 324324 32134 12.._:sdsaöüö+", 1, 1);
	createChannelTestCnt("#123 #214314 312321 ***321 ", 1, 1);
	createChannelTestCnt("    #123 ,#123", 1, 1);
	createChannelTestCnt("324983290ruhf0i#wjhßßßr12ßeoßopeqwßü,#123", 1, 1);
	createChannelTestCnt("&123,&13123213 123 123", 2, 2);
	createChannelTestCnt("213123,123213,12321 #123213,&312321", 0, 0);
	createChannelTestCnt("#,#,#,#,#,#,#,#,#,#,#", 1, 1);
	createChannelTestCnt("&,&,&,&", 1, 1);
	createChannelTestCnt(":&,:&,:&,:& :2312312    dsdsa asdasd sadasd", 1, 1);
	ok();
}

void	TestServer::joinInviteOnlyChannel()
{
	TestServer	serv;
	Channel		*channel;
	Client		*elena;
	Client		*sara = serv.createUserAndChannelRunMode("#channel", "sara", "#channel +i", 5);

	serv.makeUser("elena", 6);
	serv.runInvite(sara, "elena    #channel");
	channel = serv.getChannel("#channel");
	elena = serv.getClient("elena");


	if (channel->isInvited(elena) == false)
		return (fail("elena should be invited"));
	if (channel->getInviteOnly() == false)
		return (fail("channel should be invite only"));

	serv.runJoin(elena, "#channel");
	
	if (channel->isInChannel(elena) == false)
		return (fail("elena should be in channel"));
	if (channel->isInvited(elena) == true)
		return (fail("elena shouldn't be invited anymore"));
	
	ok();
}

void	TestServer::joinAboveUserLimit()
{
	TestServer	serv;
	Channel		*channel;
	Client		*elena;
	Client		*sara = serv.createUserAndChannelRunMode("#channel", "sara", "#channel +l 1", 5);

	serv.makeUser("elena", 6);
	serv.runInvite(sara, "elena #channel");
	channel = serv.getChannel("#channel");
	elena = serv.getClient("elena");

	if (channel->getMaxClients() != 1)
		return (fail("user limit should be 1"));
	if (channel->isInvited(elena) == false)
		return (fail("elena should be invited"));

	serv.runJoin(elena, "#channel");
	
	if (channel->isInChannel(elena) == false)
		return (fail("elena should be in channel"));
	if (channel->isInvited(elena) == true)
		return (fail("elena shouldn't be invited anymore"));
	
	ok();
}

void	TestServer::joinAboveUserLimitInvited()
{
	TestServer 	serv;
	Channel		*channel;
	Client		*sara;
	Client		*op;

	for (int i = 0; i < MAX_CLIENTS; i++)
		serv.makeUserJoinChannel("#channel", std::to_string(i), 5 + i);
	serv.makeUser("sara", 109);

	channel = serv.getChannel("#channel");
	sara = serv.getClient("sara");
	op = serv.getClient("0");
	serv.runInvite(op, "sara #channel");

	if (channel->isInvited(sara) == false)
		return (fail("sara should be invited"));
	
	serv.runJoin(sara, "#channel");
	if (sara->isInChannel(channel) == true)
		return (fail("channel shouldn't be in channel-vector"));
	if (channel->isInChannel(sara) == true)
		return (fail("sara shouldn't be in client-vector"));

	serv.runMode(op, "#channel +i");
	if (channel->isInvited(sara) == false)
		return (fail("sara should still be invited"));

	serv.runKick(op, "#channel 1");
	serv.runJoin(sara, "#channel");
	if (sara->isInChannel(channel) == false)
		return (fail("channel should be in channel-vector"));
	if (channel->isInChannel(sara) == false)
		return (fail("sara should be in client-vector"));
	ok();
}


void	TestServer::join_tests()
{
	std::cout << "\033[1;33m---JOIN TESTS---\033[0m" << std::endl;

	std::cout << "making new channel: ";
	makingNewChannel();
	std::cout << "adding too many users: ";
	addingTooManyUsers();
	std::cout << "adding list of channels: ";
	addingListOfChannels();
	std::cout << "checking channel names: ";
	CheckingChannelNames();
	std::cout << "checking wrong list input: ";
	CheckingWrongListInput();
	std::cout << "add channel with pw and join: ";
	addChannelWithPWandJoin();
	std::cout << "wrong channel name: ";
	wrongChannelName();
	std::cout << "too long list: ";
	tooLongList();
	std::cout << "random input: ";
	random_input();
	std::cout << "join invite only channel: ";
	joinInviteOnlyChannel();
	std::cout << "join channel with full user limit: ";
	joinAboveUserLimit();
	std::cout << "join channel with full user limit but invited: ";
	joinAboveUserLimitInvited();

	std::cout << std::endl;
}
