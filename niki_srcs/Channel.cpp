#include "Channel.hpp"

Channel::Channel(Client &owner, const std::string &channel_name) : _owner(owner), _name(channel_name)
{
	_members.insert(std::pair<std::string, Client &>(owner.getUserName(), owner)); // is it nessesay to add him to the _members??
	_operators.insert(std::pair<std::string, Client &>(owner.getUserName(), owner));
}
Channel::Channel(const Channel &C) : _operators(C._operators), _members(C._members), _name(C._name), _owner(C._owner)
{
	
}
Channel::~Channel() {}
									//maybe it cloud just be the user name
void	Channel::sendMsgToChannel(Client &sender, const std::string &msg)
{
	for(members_itr member = _members.begin(); member != _members.end(); member++)
	{
		if(member->second.getUserName() == sender.getUserName())
			continue ;
		if(send(member->second.getClientFd(), msg.c_str(), 513, 0) == -1)						//flags?
		{
			std::cout << "Error Send faild class Channel sendMsgToChannel" << std::endl;
			std::exit(EXIT_FAILURE);														// should it exit ? I don't think so
		}
	}
}

void	Channel::kickFromChannel(Client &executor, Client &kick_client)
{
	members_itr member;		// member that will be kicked

	if(_operators.find(kick_client.getUserName()) == _operators.end())
	{
		std::cout << ">>? kickFromChannel() executor is not a operator" << std::endl;
		return ;
	}
	member = _members.find(kick_client.getUserName());
	if(member == _members.end())
		std::cout << ">>? kickFromChannel() did not find the clinet to kick from channel: " << _name << std::endl;		// should we send a msg back that the client was not found to kick
	else
		_members.erase(member);
}

void	Channel::addClient(Client &new_member)
{
	// ### new_member.addChannel();				// add in client class the channel #############
	_members.insert(std::pair<std::string, Client &>(new_member.getNickName(), new_member));
}
