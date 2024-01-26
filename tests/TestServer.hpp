#pragma once

#include "../Includes/Irc.hpp"

class TestServer: public Irc
{
public:
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
	void	AddServerWithPWandJoin();



private:
	void	makeUserJoinChannel(const std::string channel_name, const std::string client_name, int client_fd);
	void	fail(const std::string msg) const;
	void	ok() const;
};