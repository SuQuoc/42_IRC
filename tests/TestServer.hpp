#pragma once

#include "../Includes/Irc.hpp"

class TestServer: public Irc
{
private:
	void	makeUserJoinChannel(const std::string channel_name, const std::string client_name, int client_fd);
	void	makeUser(const std::string client_name, int client_fd);
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

//---part---
	void	part_tests();
	void	partEmptySstream();
	void	partEmptySpacesSstream();
	void	partChannelNotEmpty();
	void	partChannelEmpty();
	void	fromTooManyUsersToChannelEmpty();
	void	userNotInChannel();
	void	noSuchChannel();
	void	partMultipleChannels();
	void	partOverListLimit();

//---kick---
	void	kick_tests();
	void	kickEmptySstream();
	void	kickEmptySpacesSstream();
	void	kickChannelMember();
	void	unauthorizedKick();
	void	kickSelfAndOnlyMember();
	void	userToKickNotInChannel();
	void	userToKickNotInMap();
	void	channelNotInMap();
	void	senderNotInChannel();

//---registration---
	void	registration_tests();
	void	correctPW();
	void	incorrectPW();
	void	availableNick();
	void	unavailableNick();
	void	wrongNick();
	void	registerTwoClients();

//---oper---
	void	oper_tests();
	void	wrongOperHost();
	void	wrongOperPW();
	void	correctOper();
	void	correctOperDefault();
	void	twoCorrectOper();

//---quit---
	void	quit_tests();
	void	quitWithMessage();
	void	quitWithoutMessage();

//---kill---
	void	kill_tests();
	void	succesfulKill();
	void 	notAnIrcOper();

//---client---
	void	client_tests();
	void	testMaxChannelsInClient();
	void	testLeavingOverMaxChannels();
};