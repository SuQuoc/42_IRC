
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
#include "utils.hpp"


//git checkout -b quocsu

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
	std::string _prefix;
	std::string _msg_buf;

	std::vector<Channel *> _channels;

	bool _is_registered; //?
	bool _is_authenticated;

	Client(const Client&);
    Client& operator=(const Client&);
public:
	Client(int socketFd);
	~Client();

	bool isRegistered() const;
	bool isAuthenticated() const;



	int setNickname(const std::string& name);
	void setUser(std::string& uname, const std::string& hname, const std::string& sname, const std::string& rname);
	
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
	void sendTo(const std::string& msg, Channel* recipient) const;
};


