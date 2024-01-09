#include "../Includes/Channel.hpp"

Channel::Channel(Client *owner, const std::string &channel_name) : _name(channel_name)
{
	_operators.push_back(owner);
	// owner->joinChannel(this);	// is it nessesay to add him to the _members??
}
Channel::Channel(const Channel &C) : _operators(C._operators), _members(C._members), _name(C._name), _password(C._password), _topic(C._topic) 
{
	
}
Channel::~Channel() {}
									//maybe it cloud just be the user name
void Channel::sendMsgToChannel(const Client *sender, const std::string &msg)
{
	/* for(members_itr member = _members.begin(); member != _members.end(); member++)
	{
		if(member->second->getUserName() == sender.getUserName())
			continue ;
		if(send(member->second->getClientFd(), msg.c_str(), 513, 0) == -1)					//flags?
		{
			std::cout << "Error Send faild class Channel sendMsgToChannel" << std::endl;
			std::exit(EXIT_FAILURE);													// should it exit ? I don't think so
		}
	} */
}

void Channel::rmClientFromChannel(const Client *executor, Client &rm_client)
{
	/* members_itr member;		// member that will be kicked

	(void)executor;			// ? check if executer has the rights
	if(_operators.find(kick_client.getUserName()) == _operators.end())
	{
		std::cout << ">>? kickFromChannel() executor is not a operator" << std::endl;
		return ;
	}
	member = _members.find(kick_client.getUserName());
	if(member == _members.end())
		std::cout << ">>? kickFromChannel() did not find the clinet to kick from channel: " << _name << std::endl;		// should we send a msg back that the client was not found to kick
	else
		_members.erase(member); */
}

//add clinet checks if clinet exists and add him to operator if wanted
void	Channel::addMember(Client *new_client)
{
	/* if(_members.find(new_client->getUserName()) == _members.end())
		_members.insert(std::pair<std::string, Client *>(new_client->getNickName(), new_client));
	else
		std::cout << "? could not add clinet it is already a member" << std::endl; */
}

void	Channel::addOperator(Client *new_operator)
{
	/* if(is_operator == true && _operators.find(new_client->getUserName()) == _operators.end())
		_operators.insert(std::pair<std::string, Client *>(new_client->getNickName(), new_client));
	else
		std::cout << "? could not add clinet it is already a operator" << std::endl; */
}

bool Channel::isOperator(const Client *client)
{
/* 	
	for(clients_itr itr_client; itr_client != _operators.end(); itr_client++)
	{
		if(itr)
	}
	return true; 
*/
}

//			seter
void	Channel::setName(const std::string &name) { _name = name; }
void	Channel::setPassword( const std::string &password ) { _password = password; }

//			geter
const std::string &Channel::getPassword() const { return _password; };
const std::string &Channel::getName() const { return _name; };
