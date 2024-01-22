
# pragma once

#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <iostream>
#include <algorithm>

#include "Channel.hpp"
#include "Irc_error.hpp"
#include "utils_string.hpp"

class Channel;

class Client
{
private:
	const int _fd;
	std::string _nickname; // values are change-able so not const take a look what can be changed
	std::string _username;
	std::string _hostname;
	std::string _servername;
	std::string _realname;
	std::string _prefix; //:nick!user@severname
	std::string _msg_buf;

	std::vector<Channel *> _channels;

	bool _authenticated;
	bool _registered; //?
	bool _server_op;

	Client(const Client&);
    Client& operator=(const Client&);
public:
	Client(int socketFd);
	~Client();

	bool isRegistered() const;
	bool isAuthenticated() const;
	bool isServerOp() const;

	int setNickname(const std::string& name);
	int setUser(std::string& uname, const std::string& hname, const std::string& sname, const std::string& rname);
	
	const int& getFd() const;
	const std::string& getNickname() const;
	const std::string& getUsername() const;
	// const std::string& Client::getRealname() const //?? needed or :realname enough
	const std::string& getPrefix() const; //nickname!username@hostname
	const std::vector<Channel *>& getAllChannels() const;

	void authenticate ();
	void deauthenticate();

	void joinChannel(Channel *channel); // i or fiona(const) could call then nikis function
	void leaveChannel(Channel *channel); // i or fiona(const) could call then nikis function 
	void sendTo(const std::string& msg, Client* recipient) const;

	void loadMsgBuf(const std::string& buf);
	std::string	readMsgBuf() const;
};


