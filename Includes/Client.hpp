
# pragma once

#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <iostream>

#include "Channel.hpp"
#include "Irc_error.hpp"


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

	std::vector<Channel *> _channels;

	bool _is_registered;
	bool _is_authenticated; //not needed i think

	Client(const Client&); //
    Client& operator=(const Client&); //
public:
	Client(int socketFd);
	~Client();

	bool isRegistered() const;
	// bool isAuthenticated() const; //für PASS command



	void setNickname(const std::string& name);
	void setUser(const std::string& uname, const std::string& hname, const std::string& sname, const std::string& rname);
	//void setClientInfo(const std::string& nName, const std::string& uName, const std::string& hName, const std::string& rName);
	void setToAuthenticated(); //write it


	// const std::string& getHostname() const;
	// const std::string& getRealname() const;
	const int& getFd() const;
	const std::string& getNickname() const;
	const std::string& getUsername() const;
	const std::string& getPrefix() const; //nickname!username@hostname
	const std::vector<Channel *>& getAllChannels() const;


	void joinChannel(Channel *channel); // i or fiona(const) could call then nikis function
	void leaveChannel(Channel *channel); // i or fiona(const) could call then nikis function 
	void sendTo(const std::string& msg, Client* recipient) const

};


