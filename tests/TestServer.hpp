#pragma once

#include "../Includes/Irc.hpp"

class TestServer: public Irc
{
private:
	Client*	makeUserJoinChannel(const std::string &channel_name, const std::string &client_name, int client_fd);
	Client*	makeUser(const std::string client_name, int client_fd);
	void	createChannelTestCnt(std::string join_input, size_t ch_max_Aserver, size_t ch_max_client);
	void	fail(const std::string msg) const;
	void	ok() const;
	int		runJoin(Client *client, const std::string& msg);
	int		runKick(Client *client, const std::string& msg);
	int		runInvite(Client *client, const std::string& msg);
	void	runMode(Client *client, const std::string &line);
	void	runPart(Client *client, const std::string &line);
	void	runKill(Client *client, const std::string &line);
	void	runOper(Client *client, const std::string &line);
	void	runUser(Client *client, const std::string &line);
	void	runNick(Client *client, const std::string &line);
	void	runPass(Client *client, const std::string &line);
	void	runQuit(Client *client, const std::string &line);
	Client*	createUserAndChannelRunMode(const std::string &channelname, const std::string &username, const std::string &line, int fd);


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
	void	joinInviteOnlyChannel();
	void	joinAboveUserLimit();
	void	joinAboveUserLimitInvited();

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

//---mode---
	void	mode_tests();
	void	basicTest();
	void	tTest(const std::string &mode);
	void	iTest(const std::string &mode);
	void 	lTest();
	void	runModeCheckOperator(Client *client, Client *client_target, Channel *ch, std::string run_line, std::string error_msg, bool trigger_error, bool &error);
	void	oTest();
	void	kTest();
};