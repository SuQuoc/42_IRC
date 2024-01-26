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
	ok();
}

void TestServer::join_tests()
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

	std::cout << std::endl;
}
