
# include "Client.hpp"

Client::Client(int socketFd): _connecSocket(socketFd)
{
	std::cout << "Client with socket: " << _connecSocket << "created" << std::endl;
}

Client::~Client()
{
	if (close(_connecSocket) == -1)
	{
		perror("close:");
	}
}

bool Client::isRegistered() const
{
	if (!_nickname.empty() && !_username.empty())
		return true;
	return false;
}

// bool Client::isAuthenticated() const 
// {
	// 
// }


void Client::setNickname(const std::string& name)
{
	_nickname = name;
	_prefix = ":" + _nickname + "!" + _username + "@" + _hostname; //bissi blöd but working
}

void Client::setUser(const std::string& u_name, const std::string& h_name, const std::string& r_name)
{
	_username = u_name;
	_hostname = h_name;
	_realname = r_name;

	//Annahme das NICK immer zuerst gemacht wird aber dann ist dann nc nicht gecovered falls wirs covern müssen
	_prefix = ":" + _nickname + "!" + _username + "@" + _hostname;
}

// const std::string& Client::getUsername() const {return _username;}
// const std::string& Client::getHostname() const {return _hostname;}
// const std::string& Client::getRealname() const {return _realname;}
const std::string& Client::getNickname() const {return _nickname;}
const std::string& Client::getUsername() const {return _username;}


const std::string& Client::getPrefix() const
{
	return (":" + _nickname + "!" + _username + "@" + _hostname);
}


void Client::joinChannel(Channel *channel)
{
	_channels.push_back(channel);
}

void Client::leaveChannel(Channel *channel)
{
	_channels.erase(std::find(_channels.begin(), _channels.end(), channel));
}
