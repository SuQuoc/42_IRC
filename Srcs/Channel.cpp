#include "../Includes/Channel.hpp"

//con- and destructors
Channel::Channel(Client *owner, const std::string &channel_name) :
_name(channel_name),
_restrict_topic(true),
_invite_only(false),
_max_clients(MAX_CLIENTS)
{
	if (owner == NULL)
	{
		std::cerr << "* Error: owner is NULL (in channel constructor)" << std::endl;
		return ;
	}
	addClient(owner, "", true);
}
Channel::Channel(const Channel &C) : _clients(C._clients), _password(C._password), _topic(C._topic), _name(C._name)
{

}
Channel::~Channel() {}

//private
std::vector<Channel::Member_t>::iterator Channel::getClient(const Client *client)
{
	for (clients_itr itr = _clients.begin(); itr != _clients.end(); itr++)
		if(itr->members == client)
			return itr;
	return _clients.end();
}

std::vector<Channel::Member_t>::iterator Channel::getClient(const std::string &name)
{
	for (clients_itr itr = _clients.begin(); itr != _clients.end(); itr++)
		if (itr->members->getNickname() == name)
			return itr;
	return _clients.end();
}

Channel::invited_itr	Channel::getInvited(const Client *client)
{
	if (client == NULL)
		return (_invited.end());
	return (std::find(_invited.begin(), _invited.end(), client));
}


int Channel::addInvited(Client *client)
{
	if (client == NULL)
		return (-1);
	if (isInvited(client) == false)
		_invited.push_back(client);
	return (0);
}

bool	Channel::isInvited(const Client*client)
{
	if (getInvited(client) == _invited.end())
		return (false);
	return (true);
}

//return true if user is member of this channel
bool Channel::isInChannel(const Client *client)
{
	if (client == NULL)
	{
		std::cout << "Error client is NULL isInChannel()" << std::endl;
		return false;
	}
	if (getClient(client) == _clients.end())
		return false;
	return true;
}

bool	Channel::isOperator(const Client *client)
{
	if (getClient(client)->is_operator == true)
		return true;
	return false;
}


// -1 if client is NULL
// -2 is already in channel
int	Channel::addClient(Client *new_client, const std::string &password, bool is_operator)
{
	struct Member_t member;
	bool			invited = isInvited(new_client);

	if (new_client == NULL)
		return (-1); // -1 if client is NULL
	if (getClient(new_client) != _clients.end())
		return (-2); // -2 is already in channel
	if (invited == false && _invite_only == true)
		return (ERR_INVITEONLYCHAN);
	if (size() >= MAX_CLIENTS || (invited == false && size() >= _max_clients))
		return (ERR_CHANNELISFULL);
	if (_password.empty() == false && _password != password)
		return ERR_BADCHANNELKEY;
	member.is_operator = is_operator;
	member.members = new_client;
	_clients.push_back(member);
	if (invited == true)
		_invited.erase(getInvited(new_client));
	return (0);
}

//does not rm client from clients._channels or the other maps in server
int	Channel::rmClient(const Client *executor, const Client *rm_client, const std::string &leaving_msg)
{
	clients_itr itr;

	if (!executor || !rm_client)
		return (-2);  //should never happen
	itr = getClient(executor);
	if (itr == _clients.end())
		return (ERR_NOTONCHANNEL);
	if (itr->is_operator == false)
		return ERR_CHANOPRIVSNEEDED;
	return (rmClient(rm_client, leaving_msg));
}


//returns -1 if last client leaves channel
int	Channel::rmClient(const Client *rm_client, const std::string &leaving_msg) //add
{
	clients_itr itr;

	if (!rm_client)
		return (-2); //should never happen
	itr = getClient(rm_client);
	if (itr == _clients.end())
		return (ERR_USERNOTINCHANNEL);
	sendMsg(NULL, leaving_msg);
	_clients.erase(itr);
	if (_clients.empty())
		return (DELETE_CHANNEL);
	return (0);
}

int	Channel::rmClientIgnore(Client *rm_client, Client *ignore_me, const std::string &leaving_msg)
{
	clients_itr itr;

	if (!rm_client)
		return (-2); //should never happen
	itr = getClient(rm_client);
	if (itr == _clients.end())
		return (ERR_NOTONCHANNEL);
	sendMsg(ignore_me, leaving_msg);
	_clients.erase(itr);
	if (_clients.empty())
		return (DELETE_CHANNEL);
	return (0);
}

void	Channel::sendWhoMessage(Client *sender, const std::string server_name)
{
	std::string premsg = ":" + server_name + " 353 " + sender->getNickname() + " = " + _name;
	std::string	msg = premsg;
	int			name_cnt = 0;

	for (clients_itr it = _clients.begin(); it != _clients.end(); it++)
	{
		msg += " ";
		if (it->is_operator)
			msg += "@";
		msg += it->members->getNickname();
		name_cnt++;
		if (name_cnt == 30)
		{
			protectedSendChannel(sender, msg);
			msg = premsg;
			name_cnt = 0;
		}
	}
	if (name_cnt != 0)
		protectedSendChannel(sender, msg);
	protectedSendChannel(sender, ":" + server_name + " 366 " + sender->getNickname() + " " + _name + " :End of /NAMES list.");
}

//			setter
void	Channel::setName(const std::string &name) { _name = name; }
void	Channel::setMaxClients(const int &max_clients){ _max_clients = max_clients; }

int		Channel::setMaxClients(const std::string &str, const char &pre_fix)
{
	std::stringstream sstream(str);
	int max_client;

	if(pre_fix == '-')
	{
		if(_max_clients == MAX_CLIENTS)
			return (0);
		_max_clients = MAX_CLIENTS;
		return (MODE_SET_MINUS);
	}
	if(str.empty() == true)
		return (ERR_NEEDMOREPARAMS);
	sstream >> max_client;
	if(max_client < 1 || max_client > MAX_CLIENTS || _max_clients == max_client)
		return (0);
	_max_clients = max_client;
	return (MODE_SET_PLUS);
}

int	Channel::setPassword(const std::string &password, const char &add)
{
	if(password.empty() == true)
		return (ERR_NEEDMOREPARAMS);
	if (add == '+' && _password.empty() == true) // send msg if already set
	{
		_password = password;
		return (MODE_SET_PLUS);
	}
	if (add == '-' && password == _password)
	{
		_password.clear();
		return (MODE_SET_MINUS);
	}
	return (ERR_KEYSET); // 467 ERR_KEYSET
}

//			getter

const std::string &Channel::getPassword() const { return _password; }
const std::string &Channel::getTopic() const { return _topic; }
std::string Channel::getName() const { return _name; }
bool Channel::getRestrictTopic() const { return _restrict_topic; }
bool Channel::getInviteOnly() const { return _invite_only; }
int Channel::getMaxClients() const { return _max_clients; }
int Channel::size() const { return _clients.size(); }

//returns -1 if not a + or - in add, otherwise error code if fails
int Channel::changeMode(const char &add, bool &modes)
{
	if (add == '+' && modes == false)
	{
		modes = true;	// 324     RPL_CHANNELMODEIS
		return MODE_SET_PLUS;
	}
	else if (add == '-' && modes == true)
	{
		modes = false;	// 324     RPL_CHANNELMODEIS
		return MODE_SET_MINUS;
	}
	return (CH_SUCCESS);
}

//success = 1001 TOPIC_SET
int	Channel::setTopic(const std::string &name, const std::string &topic)
{
	clients_itr client;

	client = getClient(name);
	if (client == _clients.end())
		return ERR_NOTONCHANNEL;
	if (_restrict_topic == false)
	{
		_topic = topic;
		return TOPIC_SET;
	}
	if (_restrict_topic == true && client->is_operator == true)
	{
		_topic = topic;
		return TOPIC_SET;
	}
	return ERR_CHANOPRIVSNEEDED;
}

int Channel::setOperator(const char &add, Client *client)
{
	clients_itr client_it;

	if(!client)
		return ERR_NOSUCHNICK;
	client_it = getClient(client);
	if (client_it == _clients.end())
		return (ERR_USERNOTINCHANNEL);
	if (add == '+' && client_it->is_operator == false)
	{
		client_it->is_operator = true;
		return (MODE_SET_PLUS);
	}
	else if (add == '-' && client_it->is_operator == true)
	{
		client_it->is_operator = false;	// 324     RPL_CHANNELMODEIS
		return (MODE_SET_MINUS);
	}
	return (CH_SUCCESS);
}

int Channel::setTopicOrInv(const char &add, const char &ch_modes)
{
	enum color { SET_RESTRICT_TOPIC = 't', SET_INVITE_ONLY = 'i' };

	switch (ch_modes)
	{
	case SET_RESTRICT_TOPIC:
		return changeMode(add, _restrict_topic);
	case SET_INVITE_ONLY:
		return changeMode(add, _invite_only);
	default:
		std::cerr << "Error wrong mode!" << ch_modes << std::endl;
	}
	return (0);
}

// if sender NULL send to all
int Channel::sendMsg(Client *sender, const std::string &msg)
{
	for (clients_itr itr = _clients.begin(); itr != _clients.end(); itr++)
	{
		if (sender && itr->members == sender)
			continue ;
		protectedSendChannel(itr->members, msg);
	}
	return (0);
}

//if failed returns -1
void	Channel::protectedSendChannel(Client *client, std::string msg)
{
	msg += "\r\n";
	if(client->getPipe() == true)
		return ;
	if (send(client->getFd(), msg.c_str(), msg.size(), MSG_DONTWAIT | MSG_NOSIGNAL) == -1) //MSG_DONTWAIT sets to non-block //should be nonblocking anyways because of fcntl()
        client->setPipe(true);
        /* if (errno == EPIPE) */
}
