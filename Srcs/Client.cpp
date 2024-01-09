
# include "Client.hpp"

Client::Client(int socketFd): _connecSocket(socketFd)
{
	std::cout << "Client with socket: " << _connecSocket() << "created" << std::endl;
}

Client::~Client()
{
	if (close(_connecSocket) == -1)
	{
		perror("close:");
	}
}



bool isRegistered() const
{
	
}


bool isAuthenticated() const {}


void Client::setNickName(const std::string& name)
{
	_nickname = name;
}

void Client::setUser(const std::string& u_name, const std::string& h_name, const std::string& r_name)
{
	_username = u_name;
	_hostname = h_name;
	_realname = r_name;
}

// const std::string& Client::getUsername() const {return _username;}
// const std::string& Client::getHostname() const {return _hostname;}
// const std::string& Client::getRealname() const {return _realname;}
const std::string& Client::getNickname() const {return _nickname;}


const std::string& Client::getPrefix() const
{

}


void Client::joinChannel(Channel *channel)
{
	_channels.push_back(channel);
}

void Client::leaveChannel(Channel *channel)
{
	_channels.erase(std::find(_channels.begin(), _channels.end(), channel));
}
