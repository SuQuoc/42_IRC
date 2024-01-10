#include "../Includes/Channel.hpp"

Channel::Channel(Client *owner, const std::string &channel_name) : _name(channel_name)
{
	_operators.push_back(owner);
	// owner->joinChannel(this);	// is it nessesay to add him to the _members??
}
Channel::Channel(const Channel &C) : _operators(C._operators), _members(C._members), _password(C._password), _topic(C._topic), _name(C._name)
{
	
}
Channel::~Channel() {}
									//maybe it cloud just be the user name
void Channel::sendMsg(const Client *sender, const std::string &msg)
{
	for(clients_itr member = _members.begin(); member != _members.end(); member++)
	{
		if((*member)->getUsername() == sender->getUsername())
			continue ;
		if(send((*member)->getFd(), msg.c_str(), 513, 0) == -1)					//flags?
		{
			std::cout << "Error Send faild class Channel sendMsgToChannel" << std::endl;
			std::exit(EXIT_FAILURE);													// should it exit ? I don't think so
		}
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
		_operators.erase(getOperator(rm_client->getUsername()));
		return ;
	}
	client_itr = getMember(rm_client->getUsername());
	if(client_itr != _members.end())
		_members.erase(getMember(rm_client->getUsername()));
}

//add clinet checks if clinet exists and add him to operator if wanted
void	Channel::addMember(Client *new_client)
{
	(void)new_client;
	/* if(_members.find(new_client->getUserName()) == _members.end())
		_members.insert(std::pair<std::string, Client *>(new_client->getNickName(), new_client));
	else
		std::cout << "? could not add clinet it is already a member" << std::endl; */
}

void	Channel::addOperator(Client *new_operator)
{
	(void)new_operator;
	/* if(is_operator == true && _operators.find(new_client->getUserName()) == _operators.end())
		_operators.insert(std::pair<std::string, Client *>(new_client->getNickName(), new_client));
	else
		std::cout << "? could not add clinet it is already a operator" << std::endl; */
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
