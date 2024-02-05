
# pragma once

#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <iostream>
#include <algorithm>

#include "Channel.hpp"
#include "IrcReply.hpp"
#include "utils_string.hpp"

class Channel;

class Client
{
private:
	const int _fd;
	size_t _channel_count;
	std::string _nickname; // values are change-able so not const take a look what can be changed
	std::string _username;
	std::string _hostname;
	std::string _servername;
	std::string _realname;
	std::string _prefix; //:nick!user@severname
	std::string _msg_buf;

	std::vector<Channel *> _channels;

	bool _authenticated;
	bool _registered;
	bool _server_op;
	bool broken_pipe;

	Client(const Client&);
    Client& operator=(const Client&);

	std::vector<Channel *>::iterator getChannelIter(Channel *channel);
public:


	Client(int socketFd, const std::string& ip_addr);
	~Client();

	bool isRegistered() const;
	bool isAuthenticated() const;
	bool isServerOp() const;
	bool spaceForChannel() const;
	bool isInChannel(Channel *channel) const;

	void authenticate();
	void deauthenticate();
	void elevateToServOp();

	int setNickname(const std::string& name);
	int setUser(std::string& uname, const std::string& hname, const std::string& sname, const std::string& rname);
	
	const int& getFd() const;
	int getChannelCount() const;
	const std::string& getNickname() const;
	const std::string& getUsername() const;
	const std::string& getHost() const;
	const std::string& getPrefix() const; //nickname!username@hostname
	const std::vector<Channel *>& getAllChannels() const;

	void joinChannel(Channel *channel);
	void leaveChannel(Channel *channel);

	void loadMsgBuf(const std::string& buf);
	std::string	readMsgBuf() const;

	void setPipe(bool pipe);
	bool getPipe() const;
};


