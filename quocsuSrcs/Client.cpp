
# include "Client.hpp"

Client::Client(int socketFd): _connecSocket(socketFd)
{}

Client::~Client()
{
	if (close(_connecSocket) == -1)
	{
		perror("close:");
		_exit(1);
	}
}


void Client::setClientInfo(const std::string& nName, const std::string& uName, const std::string& hName, const std::string& rName)
{
	_nickname = nName;
	_username = uName;
	_hostname = hName;
	_realname = rName;
}

void Client::setNickName(const std::string& name)
{
	_nickname = name;
}

const std::string& Client::getNickname() const {return _nickname;}
const std::string& Client::getUsername() const {return _username;}
const std::string& Client::getHostname() const {return _hostname;}
const std::string& Client::getRealname() const {return _realname;}
