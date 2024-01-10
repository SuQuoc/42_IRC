
# pragma once

#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <iostream>

#include "Channel.hpp"


//git checkout -b quocsu

class Client
{
private:
	const int _fd;
	std::string _nickname; // values are change-able so not const take a look what can be changed
	std::string _username;
	std::string _hostname;
	std::string _realname;
	std::string _prefix;

	std::vector<Channel *> _channels;

	Client(const Client&); //
    Client& operator=(const Client&); //
public:
	Client(int socketFd);
	~Client();

	bool isRegistered() const;
	// bool isAuthenticated() const; //für PASS command

	void setNickname(const std::string& name);
	void setUser(const std::string& Uname, const std::string& Hname, const std::string& Rname);
	//void setClientInfo(const std::string& nName, const std::string& uName, const std::string& hName, const std::string& rName);
	// void setUserName(const std::string& name);
	// void setHostname(const std::string& name);
	// void setRealName(const std::string& name);


	// const std::string& getHostname() const;
	// const std::string& getRealname() const;
	const int& getFd() const;
	const std::string& getNickname() const;
	const std::string& getUsername() const;
	const std::string& getPrefix() const; //nickname!username@hostname
	const std::vector<Channel *>& getAllChannels() const;


	void joinChannel(Channel *channel); // i or fiona(const) could call then nikis function
	void leaveChannel(Channel *channel); // i or fiona(const) could call then nikis function 
};


