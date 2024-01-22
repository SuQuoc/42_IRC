# include "../Includes/Client.hpp"

Client::Client(int fd): _fd(fd), _authenticated(false), _server_op(false)
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

bool Client::isServerOp() const {return _server_op;}
bool Client::isAuthenticated() const {return _authenticated;}
bool Client::isRegistered() const {return _registered;}

void Client::authenticate() {_authenticated = true;}
void Client::deauthenticate() {_authenticated = false;}

int Client::setNickname(const std::string& name)
{
	if (name.size() > 9 || containsForbiddenChars(name, " ,*?!@$:.#"))
	{
		std::cout << "432 Erreonous nickname" << std::endl;
		return ERR_ERRONEUSNICKNAME;
	}
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
	_hostname = hname;
	_realname = rname;
	_servername = sname;

	//Annahme das NICK immer zuerst gemacht wird aber dann ist dann nc nicht gecovered falls wirs covern müssen
	_prefix = _nickname + "!" + _username + "@" + _hostname;
	if (_authenticated && !_nickname.empty() && !_username.empty())
		_registered = true;
	return 0;
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
	if (!channel) 
	{
		std::cout << "Error: joinChannel()" << std::endl;
		return ;
	}
	_channels.push_back(channel);
}

void Client::leaveChannel(Channel *channel)
{
	if (!channel) 
	{
		std::cout << "Error: leaveChannel()" << std::endl;
		return ;
	}
	_channels.erase(std::find(_channels.begin(), _channels.end(), channel));
}

void Client::sendTo(const std::string& msg, Client* recipient) const //should be done by the client??
{
	// std::cout << "SEND FUNCTION NOT COMPLETED YET" << std::endl;
	std::cout << "--> Sending: " << msg << std::endl;
	if (send(recipient->getFd(), msg.c_str(), msg.size(), 0) == -1)
		std::cerr << "send() failed" << std::endl;
}

//we want to handle 
//"text\ntext^D"
//"text^D"
//"text^Dtext\n"
// buffer nur clearn und überschreiben wenn eine nachricht mit \r oder \n am ende drinnen steht. If buffer doesn't have delimiter, dranstückeln bis delimiter kommt. Wenn buffer voll, nicht überschreiben bis message mit delimiter kommt (? last sentence)
void Client::loadMsgBuffer(const std::string& buf)
{
	if (buf.find('\r') || buf.find('\n'))
		return ;
		//  std::string substring = originalString.substr(7, 5);  // Starting at index 7, take 5 characters
    // originalString.erase(7, 5);
	// else if ()		
	// _msg_buf += buf;
	// if (_msg_buf.size() > 512)
		// _msg_buf.resize(512);
	// else if (_msg_buf.size() == 512)
	// {
		// char &lastREF = _msg_buf.back();
		// if (lastREF  != '\r' && lastREF != '\n')
	// }
}

std::string Client::returnRequest()
{
	std::string request = _msg_buf;
	_msg_buf.clear();
	return request;
}
