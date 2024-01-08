
# pragma once

#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>

#include <unistd.h>

class Client
{
private:
	std::string _nickname; // values are change-able so not const take a look what can be changed
	std::string _username;
	std::string _hostname;
	std::string _realname;
	const int _connecSocket;

	Client(const Client&);
    Client& operator=(const Client&);

public:
	Client(int socketFd);
	~Client();

	void setClientInfo(const std::string& nName, const std::string& uName, const std::string& hName, const std::string& rName);
	void setNickName(const std::string& name);
	void setUserName(const std::string& name);
	void setHostname(const std::string& name);
	void setRealName(const std::string& name);

	const std::string& getNickname() const;
	const std::string& getUsername() const;
	const std::string& getHostname() const;
	const std::string& getRealname() const;
};





