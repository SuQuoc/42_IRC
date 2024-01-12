#include "../Includes/Channel.hpp"

Channel::Channel(Client *owner, const std::string &channel_name) : _name(channel_name), _max_clients(MAX_CLIENTS)
{
	if(owner == NULL)
	{
		std::cerr << "* Error: owner is NULL (in channel constructor)" << std::endl;
		return ;
	}
	addClient(owner, true);
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
		sendNonBlock(sender->getFd(), msg);
	}
}

// need to rework !!! ???
void Channel::sendNonBlock(const int &fd, const std::string &msg)
{
	//need to check if msg is not bigger than 512
	while(send(fd, msg.c_str(), msg.size(), 0) == -1)
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			continue ;
		std::cerr << "send faild in channel.cpp" << std::endl;
		strerror(errno);
		std::exit(EXIT_FAILURE);
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
		return ;
	}
	if(itr->is_operator == false)		// need to send a msg to the client ?
	{
		std::cout << "Placeholder in rmClient() in channel.hpp" << std::endl << "Executor is not a operator this channel!" << std::endl;
		return ;
	}
	rmClient(rm_client);
	//need to  send a msg to the executor?
}

void Channel::rmClient(const Client *rm_client)
{
	clients_itr itr;

	if(!rm_client)
		return ;
	itr = getClient(rm_client);
	if(itr == _clients.end())			// need to send a msg to the client ?
	{
		std::cout << "Placeholder in rmClient() in channel.hpp" << std::endl << "rm_client is not in channel!" << std::endl;
		return ;
	}
	_clients.erase(itr);
}

void	Channel::addClient(Client *new_client, bool is_operator)
{
	if(new_client == NULL)
	{
		std::cerr << "Error addMember(): new_client is NULL" << std::endl;
		return ;
	}
	if(size() >= _max_clients)
	{
		std::cerr << "Error channel" << _name << " is full" << std::endl;
		return ;
	}
	if(getClient(new_client) == _clients.end())
	{
		std::cerr << "Error client" << new_client->getUsername() << " is already in this channel." << std::endl;
		return ;
	}
	//	need to send a msg to the client ?
	struct Member_t member;
	member.is_operator = is_operator;
	member.members = new_client;
	_clients.push_back(member);
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
	if(getClient(client) == _clients.end())
		return false;
	return true;
}

//			setter
void	Channel::setName(const std::string &name) { _name = name; }
void	Channel::setMaxClients(const int &max_clients) { _max_clients = max_clients; }
// ? should it respond to the client if it to big?
void	Channel::setPassword(const std::string &password) { _password = password; }

//			getter
std::vector<Channel::Member_t>::iterator Channel::getClient(const Client *client)
{
	for(clients_itr itr = _clients.begin(); itr != _clients.end(); itr++)
		if(itr->members == client)
			return itr;
	return _clients.end();
}

const std::string &Channel::getPassword() const { return _password; }
const std::string &Channel::getName() const { return _name; }
int Channel::size() const { return _clients.size(); }
