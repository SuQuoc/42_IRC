#pragma once

#include "../Includes/Irc.hpp"

class TestServer: public Irc
{
private:
	void				makeUserJoinChannel(const std::string channel_name, const std::string client_name, int client_fd);
	std::stringstream&	makeSstream(const std::string msg) const;
	void				fail(const std::string msg) const;
	void				ok() const;


public:
//con- and destructers
	TestServer() :Irc("TestServer", "password") {}
	TestServer(const std::string name, const std::string password): Irc(name, password) {}
	~TestServer() {}

	void	join_tests();
	void	makingNewChannel();
	void	addingTooManyUsers();

//---part---
	void	part_tests();
	void	partChannelNotEmpty()
};