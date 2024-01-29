#pragma once

#include "../Includes/Irc.hpp"

class TestServer: public Irc
{
private:
	void				makeUserJoinChannel(const std::string channel_name, const std::string client_name, int client_fd);
	void				fail(const std::string msg) const;
	void				ok() const;


public:
//con- and destructers
	TestServer() :Irc("TestServer", "password") {}
	TestServer(const std::string name, const std::string password): Irc(name, password) {}
	~TestServer() {}

//---join---
	void	join_tests();
	void	makingNewChannel();
	void	addingTooManyUsers();
	void	addingListOfChannels();
	void	CheckingChannelNames();
	void	addingCheckingChannelNames();
	void	CheckingAmmountOfChannels(const size_t &expected_ch_ammount, const size_t &expected_ch_ammount_in_clients, const int &fd);
	void	CheckingWrongListInput();

//---part---
	void	part_tests();
	void	partChannelNotEmpty();
	void	partChannelEmpty();
	void	fromTooManyUsersToChannelEmpty();

//---client---
	void	client_tests();

//---registration---
	void	registration_tests();
	void	correctPW();
	void	incorrectPW();
	void	availableNick();
	void	unavailableNick();
	void	wrongNick();
	void	registerTwoClients();
};