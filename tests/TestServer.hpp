#pragma once

#include "../Includes/Irc.hpp"

class TestServer: public Irc
{
private:
	void	makeUserJoinChannel(const std::string channel_name, const std::string client_name, int client_fd);
	void	makeUser(const std::string client_name, int client_fd);
	void	createChannelTestCnt(std::string join_input, size_t ch_max_Aserver, size_t ch_max_client);
	void	fail(const std::string msg) const;
	void	ok() const;


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
	void	CheckingAmmountOfChannels(const size_t &expected_ch_ammount, const size_t &expected_ch_ammount_in_clients, const int &fd);
	void	CheckingWrongListInput();
	void	addChannelWithPWandJoin();
	void	wrongChannelName();
	void	tooLongList();
	void	random_input();

//---part---
	void	part_tests();
	void	partChannelNotEmpty();
	void	partChannelEmpty();
	void	fromTooManyUsersToChannelEmpty();
	void	userNotInChannel();
	void	noSuchChannel();

//---client---
	void	client_test();
};