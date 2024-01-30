# include "../Includes/Client.hpp"

Client::Client(int fd, const std::string& ip_addr): 
_fd(fd),
_channel_count(0),
_hostname(ip_addr),
_authenticated(false),
_registered(false),
_server_op(false)
{}

Client::~Client()
{
	if (close(_fd) == -1)
	{
		/* perror("close:"); */
	}
}

bool Client::isServerOp() const {return _server_op;}
bool Client::isAuthenticated() const {return _authenticated;}
bool Client::isRegistered() const {return _registered;}
bool Client::spaceForChannel() const
{
	if (_channel_count == 10) 
		return false;
	return true;
}

bool Client::isInChannel(Channel *channel) const
{
	std::vector<Channel *>::const_iterator it = std::find(_channels.begin(), _channels.end(), channel);
	if (it == _channels.end())
		return (false);
	return (true);
}

void Client::authenticate() {_authenticated = true;}
void Client::deauthenticate() {_authenticated = false;}
void Client::elevateToServOp() {_server_op = true;}


int Client::setNickname(const std::string& name)
{
	if (name.size() > 9 || containsForbiddenChars(name, " ,*?!@$:.#"))		
		return ERR_ERRONEUSNICKNAME;
	_nickname = name;
	_prefix = _nickname + "!" + _username + "@" + _hostname; //a lil ick but working
	if (_authenticated && !_nickname.empty() && !_username.empty())
		_registered = true;
	return 0;
}
// Note that hostname and servername are normally ignored by the IRC
// server when the USER command comes from a directly connected client
// (for security reasons), but they are used in server to server
// communication.
int Client::setUser(std::string& uname, const std::string& hname, const std::string& sname, const std::string& rname)
{
	if (uname.empty() || hname.empty() || rname.empty() || sname.empty())
	{
		std::cout << "Need more params" << std::endl; //rm later
		return ERR_NEEDMOREPARAMS; //ERR_NEEDMOREPARAMS?
	}
	else if (uname.size() > 9)
		uname.resize(9); //removeed const from uname --> irc bad protocol
	_username = uname;
	_realname = rname;
	_servername = sname;
	(void)hname;

	//Annahme das NICK immer zuerst gemacht wird aber dann ist dann nc nicht gecovered falls wirs covern müssen
	_prefix = _nickname + "!" + _username + "@" + _hostname;
	if (_authenticated && !_nickname.empty() && !_username.empty())
		_registered = true;
	return 0;
}

const int& Client::getFd() const {return _fd;}
int Client::getChannelCount() const {return _channel_count;}
const std::string& Client::getNickname() const {return _nickname;}
const std::string& Client::getUsername() const {return _username;}
const std::string& Client::getHost() const {return _hostname;}
const std::string& Client::getPrefix() const {return _prefix;}
const std::vector<Channel *>& Client::getAllChannels() const {return _channels;}
std::vector<Channel *>::iterator Client::getChannelIter(Channel *channel)
{
	if (!channel)
		return (_channels.end());
	return (std::find(_channels.begin(), _channels.end(), channel));
}


void Client::joinChannel(Channel *channel)
{
	if (!channel) 
	{
		std::cout << "Error: joinChannel()" << std::endl;
		return ;
	}
	std::vector<Channel*>::iterator	it = getChannelIter(channel);
	if(it == _channels.end() && _channel_count < 10)
	{
		_channels.push_back(channel);
		_channel_count++;
	}
}

void Client::leaveChannel(Channel *channel)
{
	if (!channel) 
	{
		std::cout << "Error: leaveChannel()" << std::endl;
		return ;
	}
	std::vector<Channel*>::iterator	it = getChannelIter(channel);
	if(it == _channels.end())
		return ;
	_channels.erase(it);
	_channel_count--;
}

void	Client::loadMsgBuf(const std::string& str)
{
	if (str.empty())
		return ;
	if (_msg_buf.empty() || *(_msg_buf.end() - 1)  == '\n')
		_msg_buf = str;
	else
	{
		_msg_buf += str;
		if (_msg_buf.size() > 512)
		{
			_msg_buf.resize(512);
			if (*(str.end() - 1) == '\n')
				_msg_buf += '\n';
		}	
	}
}

std::string	Client::readMsgBuf() const
{
	std::string msg;
	if (!_msg_buf.empty() && *(_msg_buf.end() - 1) == '\n')
	{
		msg = _msg_buf;
		msg.erase((msg.end() - 1));
		return (msg);
	}
	else
		return "";
}
