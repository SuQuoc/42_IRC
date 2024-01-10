#include "../Includes/Channel.hpp"

Channel::Channel(Client *owner, const std::string &channel_name) : _name(channel_name), _max_clients(MAX_CLIENTS)
{
	if(owner == NULL)
	{
		std::cerr << "Channel constructor owner is NULL!" << std::endl;
		return ;
	}
	_operators.push_back(owner);
}
Channel::Channel(const Channel &C) : _operators(C._operators), _members(C._members), _password(C._password), _topic(C._topic), _name(C._name)
{
	
}
Channel::~Channel() {}

void Channel::sendMsg(const Client *sender, const std::string &msg)
{
	for(clients_itr member = _members.begin(); member != _members.end(); member++)
	{
		if((*member)->getUsername() == sender->getUsername())
			continue ;
		sendNonBlock(sender->getFd(), msg);
	}
}

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

//does not rm client from clients._channels
void Channel::rmClient(const Client *executor, Client *rm_client)
{
	if(isOperator(executor))
		rmClient(rm_client);
	else
		std::cout << "Placeholder in rmClient() in channel.hpp" << std::endl << "Executor is not a operator in this channel!" << std::endl;
	//need to  send a msg to the executor?
}

void Channel::rmClient(Client *rm_client)
{
	clients_itr client_itr;		// member that will be kicked

	client_itr = getOperator(rm_client->getUsername());
	if(client_itr != _members.end())
	{
		_operators.erase(client_itr);
		return ;
	}
	client_itr = getMember(rm_client->getUsername());
	if(client_itr != _members.end())
		_members.erase(client_itr);
}

void	Channel::addClient(std::vector<Client *> &vector, Client *new_client)
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
	vector.push_back(new_client);
}

void	Channel::addMember(Client *new_client)
{
	addClient(_members, new_client);
}

void	Channel::addOperator(Client *new_operator)
{
	addClient(_operators, new_operator);
}

bool Channel::isOperator(const Client *client)
{
	for(size_t i = 0; i < _operators.size(); i++)
		if(_operators[i]->getUsername() == client->getUsername())
			return false;
	return true; 
}

//			setter
void	Channel::setName(const std::string &name) { _name = name; }
void	Channel::setMaxClients(const int &max_clients) { _max_clients = max_clients; }
// ? should it respond to the client if it to big?
void	Channel::setPassword(const std::string &password) { _password = password; }

//			getter
std::vector<Client *>::iterator Channel::getOperator(const std::string &name)
{
	for(clients_itr itr = _operators.begin(); itr != _operators.end(); itr++)
		if((*itr)->getUsername() == name)
			return itr;
	return _operators.end();
}

std::vector<Client *>::iterator Channel::getMember(const std::string &name)
{
	for(clients_itr itr = _members.begin(); itr != _members.end(); itr++)
		if((*itr)->getUsername() == name)
			return itr;
	return _members.end();
}

const std::string &Channel::getPassword() const { return _password; }
const std::string &Channel::getName() const { return _name; }
int Channel::size() const { return _operators.size() + _members.size(); }
