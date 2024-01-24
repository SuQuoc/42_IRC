#include "../Includes/Channel.hpp"

Channel::Channel(Client *owner, const std::string &channel_name) : 
_name(channel_name),
_restrict_topic(true),
_invite_only(false),
_max_clients(MAX_CLIENTS)
{
	if(owner == NULL)
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

void Channel::sendMsg(const Client *sender, const std::string &msg)
{
	if(!sender)
	{
		std::cerr << "* Error: sender is NULL (in sendMsg)" << std::endl;
		return ;
	}
	for(clients_itr itr = _clients.begin(); itr != _clients.end(); itr++)
	{
		if(itr->members == sender)
			continue ;
		if(send(itr->members->getFd(), msg.c_str(), msg.size(), 0) == -1)
		{
			/* if (errno == EAGAIN || errno == EWOULDBLOCK)
				continue ; */
			std::cerr << "send faild in channel.cpp" << std::endl;
			strerror(errno);
			std::exit(EXIT_FAILURE); //?
		}
	}
}

//does not rm client from clients._channels or the other maps in server
void Channel::rmClient(const Client *executor, const Client *rm_client)
{
	clients_itr itr;

	if(!executor || !rm_client)
		return ;
	itr = getClient(executor);
	if( itr == _clients.end())			// need to send a msg to the client ?
	{
		std::cout << "Placeholder in rmClient() in channel.hpp" << std::endl << "Executor is not a in this channel!" << std::endl;
		// 441 ERR_USERNOTINCHANNEL
		return ;
	}
	if(itr->is_operator == false)		// need to send a msg to the client ?
	{
		std::cout << "Placeholder in rmClient() in channel.hpp" << std::endl << "Executor is not a operator this channel!" << std::endl;
		return ;
		// :server-name 482 your-nickname #channel :You're not channel operator ?
	}
	rmClient(rm_client); //needs pass error-codes to irc
	//need to  send a msg to the executor?
}

//returns -1 if last client leaves channel
int Channel::rmClient(const Client *rm_client) //add 
{
	clients_itr itr;

	if(!rm_client)
		return -2; //should never happen
	itr = getClient(rm_client);
	if(itr == _clients.end())			// need to send a msg to the client ?
	{
		std::cout << "Placeholder in rmClient() in channel.hpp" << std::endl << "rm_client is not in channel!" << std::endl;
		return (ERR_NOTONCHANNEL);
	}
	sendMsg(rm_client, ":" + rm_client->getPrefix() + " PART " + _name + " :leaving\r\n");
	_clients.erase(itr);
	if (_clients.empty())
		return (-1);
	return (0);
}

// -1 if client is NULL
// -2 is already in channel
int	Channel::addClient(Client *new_client, const std::string &password, bool is_operator)
{
	if(new_client == NULL)
	{
		std::cerr << "Error addMember(): new_client is NULL" << std::endl;
		return -1; // -1 if client is NULL
	}
	if(size() >= _max_clients)
		return ERR_CHANNELISFULL;
	if(getClient(new_client) != _clients.end())
	{
		std::cerr << "Error client" << new_client->getUsername() << " is already in this channel." << std::endl;
		return -2; // -2 is already in channel
	}
	if(_password.empty() == false && _password != password)
		return ERR_BADCHANNELKEY;
	struct Member_t member;
	member.is_operator = is_operator;
	member.members = new_client;
	_clients.push_back(member);
	return 0;
}

bool	Channel::isOperator(const Client *client)
{
	if(getClient(client)->is_operator == true)
		return true;
	return false;
}

//return true if user is member of this channel
bool Channel::isInChannel(const Client *client)
{	
	if(client == NULL)
	{
		std::cout << "Error client is NULL isInChannel()" << std::endl;
		return false;
	}
	if(getClient(client) == _clients.end())
		return false;
	return true;
}

//			setter
void	Channel::setMaxClients(const int &max_clients) { _max_clients = max_clients; }
void	Channel::setName(const std::string &name) { _name = name; }
// ? should it respond to the client if it to big? 
int	Channel::setPassword(Client *executor, const std::string &password, const char &add)
{
	clients_itr client;

	client = getClient(executor);
	// send msg ???? youre not a channel operator
	if(client == _clients.end() || client->is_operator == false)
		return ERR_CHANOPRIVSNEEDED; // = 481 ERR_NOPRIVILEGES
	if(add == '+' && _password.empty() == true) // send msg if already set
	{
		_password = password;
		return RPL_CHANNELMODEIS;
	}
	if(add == '-' && password == _password)
	{
		_password.clear();
		return RPL_CHANNELMODEIS;
	}
	return ERR_KEYSET; // 467 ERR_KEYSET
}
//sucess = 1001 TOPIC_SET
int	Channel::setTopic(const std::string &name, const std::string &topic)
{
	clients_itr client;

	client = getClient(name);
	if(client == _clients.end())
		return ERR_NOTONCHANNEL;
	if(_restrict_topic == false)
	{
		_topic = topic;
		return TOPIC_SET;
	}
	if(_restrict_topic == true && client->is_operator == true)
	{
		_topic = topic;
		return TOPIC_SET;
	}
	return ERR_CHANOPRIVSNEEDED;
}

//			getter
std::vector<Channel::Member_t>::iterator Channel::getClient(const Client *client)
{
	for(clients_itr itr = _clients.begin(); itr != _clients.end(); itr++)
		if(itr->members == client)
			return itr;
	return _clients.end();
}

std::vector<Channel::Member_t>::iterator Channel::getClient(const std::string _name)
{
	for(clients_itr itr = _clients.begin(); itr != _clients.end(); itr++)
		if(itr->members->getNickname() == _name)
			return itr;
	return _clients.end();
}
const std::string &Channel::getTopic() const { return _topic; }
const std::string &Channel::getPassword() const { return _password; }
const std::string &Channel::getName() const { return _name; }
bool Channel::getInviteOnly() const { return _invite_only; }
int Channel::size() const { return _clients.size(); }

//returns -1 if not a + or - in add, otherwise error code if fails
int Channel::changeMode(Client *executor, const char &add, bool &modes)
{
	if(getClient(executor)->is_operator == false)
		return ERR_NOPRIVILEGES;     //ERR_NOPRIVILEGES
	if(add == '+' && modes == false)
	{
		modes = true;	// 324     RPL_CHANNELMODEIS
		return RPL_CHANNELMODEIS;
	}
	else if(add == '-' && modes == true)
	{
		modes = false;	// 324     RPL_CHANNELMODEIS
		return RPL_CHANNELMODEIS;
	}
	return(CH_SUCCESS);
}

int Channel::setOperator(Client *executor, const char &add, const std::string &name)
{
	clients_itr itr;

	if(getClient(executor) == _clients.end())
		return ERR_NOTONCHANNEL;		// :server-name 442 your-nickname #channel :You're not on that channel
	if(getClient(executor)->is_operator == false)
		return ERR_CHANOPRIVSNEEDED;     //ERR_NOPRIVILEGES
	if(getClient(name) == _clients.end())
		return ERR_USERNOTINCHANNEL;
	if(add == '+')
	{
		if(itr->is_operator == true)
			return 491; //:server-name 491 your-nickname #channel :You're already an operator
		itr->is_operator = true;
		return RPL_CHANNELMODEIS;
	}
	else if(add == '-')
	{
		if(itr->is_operator == false)
			return 491; // :server-name 491 your-nickname #channel :They are not an operator
		itr->is_operator = true;	// 324     RPL_CHANNELMODEIS
		return RPL_CHANNELMODEIS;
	}
	return(CH_SUCCESS);
}

//multible modes a possilbe like +adasd
//ervery char needs to be handelt
//multible arguments are possible
int Channel::modesSwitch(Client *executor, const char &add, const char &ch_modes, const std::string &argument)
{
	enum color { SET_RESTRICT_TOPIC = 't', SET_INVITE_ONLY = 'i', SET_KEY = 'k', SET_OPERATOR = 'o' };
	clients_itr itr;

	switch (ch_modes)
	{
	case SET_RESTRICT_TOPIC:
		return changeMode(executor, add, _restrict_topic);
	case SET_INVITE_ONLY:
		return changeMode(executor, add, _invite_only);
	case SET_OPERATOR:
		return setOperator(executor, add, argument);
	case SET_KEY:
		return setPassword(executor, argument, add);
	default:
		std::cout << "Error wrong mode!" << ch_modes << std::endl;
	}
	/* else if(mode == 'o' && (itr = getClient(argument)) != _clients.end())
		itr->is_operator = true; */
	return 0;
}
