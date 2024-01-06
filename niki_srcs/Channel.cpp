#include "Channel.hpp"

Channel::Channel(Client &owner, const std::string &channel_name) : _owner(owner), _name(channel_name)
{
	members.insert(std::pair<std::string, Client &>(owner.getNickName(), owner)); // is it nessesay to add him to the members??
}

void	Channel::sendMsgToChannel(Client &sender, const std::string &msg)
{
	for(members_itr member = members.begin(); member != members.end(); member++)
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

void	Channel::addClient(Client &new_member)
{
	// ### new_member.addChannel();				// add in client class the channel #############
	members.insert(std::pair<std::string, Client &>(new_member.getNickName(), new_member));
}
