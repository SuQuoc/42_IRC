# include "../Includes/Client.hpp"

Client::Client(int fd): _fd(fd), _is_authenticated(false)
{
	/* std::cout << "Client with socket: " << _fd << "created" << std::endl; */
}

Client::~Client()
{
	if (close(_fd) == -1)
	{
		/* perror("close:"); */
	}
}

bool Client::isRegistered() const
{
	if (_is_authenticated && !_nickname.empty() && !_username.empty())
		return true;
	return false;
}

bool Client::isAuthenticated() const {return _is_authenticated;}

void Client::authenticate() {_is_authenticated = true;}
void Client::deauthenticate() {_is_authenticated = false;}

int Client::setNickname(const std::string& name)
{
	if (name.size() > 9 || containsForbiddenChars(name, " ,*?!@$:.#"))
	{
		std::cout << "432 Erreonous nickname" << std::endl;
		return ERR_ERRONEUSNICKNAME;
	}
	_nickname = name;
	_prefix = ":" + _nickname + "!" + _username + "@" + _hostname; //a lil ick but working
	return 0;
}

void Client::setUser(std::string& uname, const std::string& hname, const std::string& sname, const std::string& rname)
{
	if (uname.empty() || hname.empty() || rname.empty() || sname.empty())
	{
		std::cout << "Need more params" << std::endl; //rm later
		return ; //ERR_NEEDMOREPARAMS?
	}
	else if (uname.size() > 9)
		uname.resize(9); //removeed const from uname --> irc bad protocol
	_username = uname;
	_hostname = hname;
	_realname = rname;
	_servername = sname;

	//Annahme das NICK immer zuerst gemacht wird aber dann ist dann nc nicht gecovered falls wirs covern müssen
	_prefix = _nickname + "!" + _username + "@" + _hostname;
}

// const std::string& Client::getUsername() const {return _username;}
// const std::string& Client::getHostname() const {return _hostname;}
// const std::string& Client::getRealname() const {return _realname;}
const int& Client::getFd() const {return _fd;}
const std::string& Client::getNickname() const {return _nickname;}
const std::string& Client::getUsername() const {return _username;}
const std::string& Client::getPrefix() const {return _prefix;}
const std::vector<Channel *>& Client::getAllChannels() const {return _channels;}


void Client::joinChannel(Channel *channel)
{
	_channels.push_back(channel);
}

void Client::leaveChannel(Channel *channel)
{
	_channels.erase(std::find(_channels.begin(), _channels.end(), channel));
}

void Client::sendTo(const std::string& msg, Client* recipient) const
{
	const std::string message = this->getPrefix() + msg;
	// std::cout << "SEND FUNCTION NOT COMPLETED YET" << std::endl;
	if (send(recipient->getFd(), message.c_str(), message.size(), 0) == -1)
		std::cerr << "send() failed" << std::endl;
}


void Client::sendTo(const std::string& msg, Channel* recipient) const //???????????
{
	const std::string message = this->getPrefix() + " " + msg;
	std::cout << "--> Sending: " << message << std::endl;
	// std::cout << "SEND FUNCTION NOT COMPLETED YET" << std::endl;
	// if (send(recipient->getFd(), message.c_str(), message.size(), 0) == -1)
		// std::cerr << "send() failed" << std::endl;
	(void)(recipient);
}
