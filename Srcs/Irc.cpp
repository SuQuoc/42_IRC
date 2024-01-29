#include "../Includes/Irc.hpp"


//con- and destructer
Irc::Irc(const std::string& name, const std::string& password): 
AServer(name, password),
_replier(name)
{
	_op_host = "OpHost"; 
	_op_password = "OpPass";
}
/* Irc::Irc(const Irc& I);
Irc::Irc operator=(const Irc& I); */
Irc::~Irc() {}

//private methods 

void	Irc::command_switch(Client *sender, const std::string message, const int& new_client_fd) //message-> 'request' better name? for us to discern
{
	std::cout << "message =" << message << "!" << std::endl;

    std::stringstream	sstream(message); //message can't be empty
    std::string	cmd;

	//this block checks for Prefix 
	std::getline(sstream >> std::ws, cmd, ' ');
	if (cmd.at(0) == ':')
	{
		if (cmd != sender->getPrefix())
		{
			std::cout << "YOU are a imposter" << std::endl; //"n!u@" nothing after @ should work; scared bc hexchat has some weird domain after @ :@1321.32133.3213.IRC
			//send()
			return;
		}
		std::getline(sstream, cmd, ' ');
	}
	if (cmd == "CAP") //we only take the last PASS
		return ;
	else if (cmd == "PASS") PASS(sender, sstream); //we only take the last PASS // client can always try PASS although not registered ?
	else if (cmd == "NICK") NICK(sender, sstream);
	else if (cmd == "USER")	USER(sender, sstream);
	else if (sender->isRegistered() == false) _replier.sendError(ERR_NOTREGISTERED, sender, ""); //?
	else if (cmd == "PRIVMSG") PRIVMSG(sender, sstream);
	else if (cmd == "JOIN") JOIN(sender, sstream);
	else if (cmd == "PART") PART(sender, sstream);
	else if (cmd == "QUIT") QUIT(sender, sstream);
	else if (cmd == "KICK") KICK(sender, sstream);
	else if (cmd == "INVITE") std::cout << "INVITE()" << std::endl; //INVITE();
	else if (cmd == "MODE") std::cout << "MODE()" << std::endl; //MODE();
	else if (cmd == "TOPIC") TOPIC(sender, sstream);
	else if (cmd == "OPER") OPER(sender, sstream);
	else _replier.sendError(ERR_UNKNOWNCOMMAND, sender, cmd);
	std::cout << std::endl;
}

//methods (commands)
//---------------------------------COMMANDS--------------------------------------------
void Irc::PASS(Client *sender, std::stringstream &sstream)
{
    std::string password = extractWord(sstream);
    	
    if (sender->isRegistered()) 
        _replier.sendError(ERR_ALREADYREGISTERED, sender, ""); //already registered
    else if (password == _password)
		sender->authenticate();
	else
	{
		std::cout << "pw = " << password << std::endl;
		sender->deauthenticate();
		_replier.sendError(ERR_PASSWDMISMATCH, sender, "");
		//delete Client and the entry from the map if Pw is wrong? --> multiple PASS not possible then
	}
}

void Irc::NICK(Client *sender, std::stringstream &sstream)
{
    std::string nickname = extractWord(sstream);
    //if (sender->isAuthenticated() == false) //didnt do PASS before NICK 
    //    _replier.sendError(321, sender) and return; //theres no err_code for it

	client_name_map_const_it it = getClientIter(nickname); //key may not be used cuz it creates an entry --> actually good for us no?
	if (it == _client_names.end()) //no one has the nickname
	{
		if (sender->setNickname(nickname) != 0)
		{
			_replier.sendError(ERR_ERRONEUSNICKNAME, sender, "");
			return ;
		}
	}
	else
	{
		_replier.sendError(ERR_NICKNAMEINUSE, sender, "");
		return ;
	}
	if (sender->isRegistered())
	{
		addClientToNameMap(sender->getNickname(), sender->getFd());
		_replier.sendRPL(RPL_WELCOME, sender, sender->getUsername());
	}
}

void	Irc::USER(Client *sender, std::stringstream &sstream)
{
    std::vector<std::string> info(4);

	if (sender->isRegistered())
	{
		_replier.sendError(ERR_ALREADYREGISTERED, sender, "");
		return ;
	}
    for (std::vector<std::string>::iterator it = info.begin(); it != info.end(); it++)
		*it = extractWord(sstream);
    if (sender->setUser(info[0], info[1], info[2], info[3]) == ERR_NEEDMOREPARAMS)
		_replier.sendError(ERR_NEEDMOREPARAMS, sender, "");
	if (sender->isRegistered())
	{
		addClientToNameMap(sender->getNickname(), sender->getFd());
		_replier.sendRPL(RPL_WELCOME, sender, sender->getUsername());
	}
}

int	Irc::JOIN(Client *sender, std::stringstream &sstream)
{
	std::stringstream stream_name(extractWord(sstream));
	std::stringstream stream_key(extractWord(sstream));
	channel_map_iter_t channel_itr;
	std::string	channel_name;
	std::string	channel_key;

	while(getline(stream_name, channel_name, ',')) //LIST_LIMIT?
	{
		getline(stream_key, channel_key, ',');
		if(!(channel_name[0] == '#' || channel_name[0] == '&') || channel_name.size() > 200) //check if channel_name is valid
		{
			_replier.sendError(ERR_NOSUCHCHANNEL, sender, channel_name);
			continue;
		}	
		if (sender->spaceForChannel() == false)
		{
			_replier.sendError(ERR_TOOMANYCHANNELS, sender, channel_name);
			continue;
		}
		channel_itr = _channels.find(channel_name);
		if(channel_itr == _channels.end()) // create if channel non exist ?
			addNewChannelToMap(sender, channel_name);
		else
		{
			int err = channel_itr->second->addClient(sender, channel_key, false);
			if(err > 0)
			{
				_replier.sendError(static_cast<IRC_ERR>(err), sender, channel_name);
				continue;
			}
			else if(err < 0)
				continue;
		}
		channel_itr = _channels.find(channel_name);
		sender->joinChannel(channel_itr->second);
		channel_itr->second->sendMsg(NULL, ":" + sender->getPrefix() + " JOIN " + channel_name + " * :" + sender->getUsername() + "\r\n");
	}
	return (0);
}

void	Irc::PART(Client *sender, std::stringstream &sstream) //tested (not thoroughly)
{
	Channel*			channel;
	std::stringstream	channel_name_sstream(extractWord(sstream));
	std::string			channel_name;
	std::string			part_msg;
	int					cnt = 0;
	int					err;

	while (cnt < LIST_LIMIT && std::getline(channel_name_sstream, channel_name, ','))
	{
		cnt++;
		channel = getChannel(channel_name);
		if (channel == NULL)
		{
			std::cerr << "*Error: PART(): channel is not in channel map" << std::endl;
			_replier.sendError(ERR_NOSUCHCHANNEL, sender, channel_name);
			continue ;
		}
		part_msg = createMsg(sender, "PART", channel_name, "Leaving");
		err = channel->rmClient(sender, part_msg);
		if (err > 0)
		{
			std::cerr << "*Error: PART(): err > 0" << std::endl;
			_replier.sendError(static_cast<IRC_ERR>(err), sender, channel_name);
			continue ;
		}
		sender->leaveChannel(channel);
		if (err == DELETE_CHANNEL) //delete channel when empty()
			rmChannelFromMap(channel_name);
	}
	if (cnt == 0)
		_replier.sendError(ERR_NEEDMOREPARAMS, sender, "");
	if (!channel_name_sstream.eof())
		return ; //_replier.sendError(too many argument in list)!
}

//cant use PART
// - doesnt support listing with ',' e.g: "hungry,bye guys"
// - i dont write myself a message when quiting
// - only errors with quit would be if client is the last in channel
// 		or if the channel doesnt exist
void	Irc::QUIT(Client *sender, std::stringstream &sstream)
{
	std::string	channel_name;
	std::string	msg = extractWord(sstream);
	Channel		*channel; 
	int err;
	if (msg.empty())
		msg = "disconnected"; //where will the entire message be concatinated? in channel?
	
	std::vector<Channel *> channels = sender->getAllChannels();
	for (std::vector<Channel *>::iterator it = channels.begin(); it != channels.end(); it++)
	{
		channel = (*it); 
		if (!channel) //necessary? checking if the channel is in map or null seems overkill, since this case should never happen
			continue ;
		// sender->leaveChannel(channel); //unecessary ?? he will leave entire server
		channel_name = channel->getName();
		err = channel->rmClient(sender, "Leaving");
		if (err == -1) //exchange -1 with CHANNEL_DEAD, and use rmClient with a message
			rmChannelFromMap(channel_name);		
	}
	rmClientFromMaps(sender);
}

void	Irc::KICK(Client *sender, std::stringstream &sstream)
{
	Client*					user_to_kick;
	Channel*				channel;
	std::string	channel_name(extractWord(sstream));
	std::string	nickname(extractWord(sstream));
	std::string	msg(extractWord(sstream));
	int			err;

	if (msg.empty())
		msg = nickname;
	if (channel_name.empty() || nickname.empty())
	{
		_replier.sendError(ERR_NEEDMOREPARAMS, sender, "KICK");
		return ;
	}
	channel = getChannel(channel_name);
	if (channel == NULL)
	{
		_replier.sendError(ERR_NOSUCHCHANNEL, sender, channel_name);
		return ;
	}
	user_to_kick = getClient(nickname);
	if (user_to_kick == NULL)
	{
		_replier.sendError(ERR_NOSUCHNICK, sender, nickname); //ERR-NOSUCHNICK is not in list of numeric replies for kick in protocoll
		return ;
	}

	msg = ":" + sender->getPrefix() + " KICK " + channel_name + " " + nickname + " :" + msg + "\r\n";
	err = channel->rmClient(sender, user_to_kick, msg);
	if (err > 0)
	{
		std::cout << "KICK: > 0" << std::endl;
		_replier.sendError(static_cast<IRC_ERR>(err), sender, channel_name);
		return ;
	}
	sender->leaveChannel(channel);
	if (err == -1) //delete channel when empty()
		rmChannelFromMap(channel_name);
}




// ERR_NORECIPIENT
// ERR_NOTEXTTOSEND 412
// ERR_NOSUCHNICK
// ERR_CANNOTSENDTOCHAN 404 --> unecessary, mode n,m, and v not required
// ERR_TOOMANYTARGETS ?? I DONT WANT TO COVER THAT
// Hexchat doesnt allow PRIVMSG with channels
void Irc::PRIVMSG(Client *sender, std::stringstream &sstream)
{
	std::stringstream	recip_sstream(extractWord(sstream));
	std::string 		message = extractWord(sstream);
	std::string 		recipient;
	std::string			reply;
	int					cnt = 0;

	while (cnt < LIST_LIMIT && std::getline(recip_sstream, recipient, ','))
	{
		cnt++;
		if (recipient.empty()) 
			_replier.sendError(ERR_NORECIPIENT, sender, "PRIVMSG");
		else if (message.empty())
			_replier.sendError(ERR_NOTEXTTOSEND, sender, "");
		else if (recipient.at(0) == '#')
		{
			Channel *channel = getChannel(recipient);
			if (channel == NULL)
				_replier.sendError(ERR_NOSUCHCHANNEL, sender, recipient);
			else
				channel->sendMsg(sender, createMsg(sender, "PRIVMSG", recipient, message));
		} //mask?
		else
		{
			Client *reciever = getClient(recipient);
			if (reciever == NULL)
				_replier.sendError(ERR_NOSUCHNICK, sender, "");
			else
			{
				reply = createMsg(sender, "PRIVMSG", recipient, message); //PART uses same method
				std::cout << "--> Sending: " << reply << std::endl; //out!
				if (send(reciever->getFd(), reply.c_str(), reply.size(), 0) == -1)
					std::cerr << "send() failed" << std::endl;
			}
		}
	}
	if (cnt == 0)
		_replier.sendError(ERR_NORECIPIENT, sender, "PRIVMSG");
}

//void Irc::MODE(Client *sender, std::stringstream &sstream);
//void Irc::TOPIC(Client *sender, std::stringstream &sstream);
//void Irc::INVITE(Client *sender, std::stringstream &sstream);

// void Irc::clientDied(int client_fd) //pure virtual or put this as a normal function in AServer
// {
// 	std::string	channel_name;
// 	int err;
// 	Client *client = _client_fds.find(client_fd)->second;
//
//	std::vector<Channel *> channels = client->getAllChannels();
//	for (std::vector<Channel *>::iterator it = channels.begin(); it != channels.end(); it++)
//	{
//		Channel *channel = (*it); 
//		if (!channel)
//			continue ;
//		channel_name = channel->getName();
//		err = channel->rmClient(client); //"lost connection" 
//		if (err == -1)
//			rmChannelFromMap(channel_name);		
//	}
//	rmClientFromMaps(client);
//}


void Irc::OPER(Client *sender, std::stringstream &sstream)
{
	std::cout << "Executing OPER()" << std::endl;
	std::string	host = extractWord(sstream); //chose to name the string "host" and not "user" irc protocoll a bit vague
	std::string	pw = extractWord(sstream);
	
	if (host.empty() || pw.empty())
		_replier.sendError(ERR_NEEDMOREPARAMS, sender, "");
	else if (host != _op_host && sender->getHost() != _op_host)
		_replier.sendError(ERR_NOOPERHOST, sender, "");
	else if (pw != _op_password)
		_replier.sendError(ERR_PASSWDMISMATCH, sender, "");
	else
	{
		sender->elevateToServOp(); //what if send in next line fails?
		_replier.sendRPL(RPL_YOUREOPER, sender, "");
		std::cout << "INFO: " << sender->getPrefix() << " is now server op, chaos is coming!" << std::endl;
	}
}

std::string Irc::createMsg(Client *sender, const std::string& cmd, const std::string& recipient, const std::string& msg) const
{
    if (!sender)
        return msg;
    std::string message = ":" + sender->getPrefix() + " " + cmd + " " + recipient + " :" + msg + "\r\n"; //PART uses same method -> extra function?
    return message;
}

void Irc::TOPIC(Client *sender, std::stringstream &sstream)
{
	std::string channel_name = extractWord(sstream);
	std::string topic = extractWord(sstream);
	Channel* 	channel;

	if (channel_name.empty())
		return (_replier.sendError(ERR_NEEDMOREPARAMS, sender, ""), void());

	channel = getChannel(channel_name);
	if (channel == NULL)
		return ; //no error listed in protocoll
	if (topic.empty()) //only wants 
	{
		if (!channel->getTopic().empty())
		{
			std::string reply = channel_name + " :" + channel->getTopic();
			_replier.sendRPL(RPL_TOPIC, sender, reply);
		}
		else
			_replier.sendRPL(RPL_NOTOPIC, sender, channel_name);
	}
	else
	{
		int err = channel->setTopic(sender->getNickname(), topic);
		if (err != 0)
			_replier.sendError(static_cast<IRC_ERR>(err), sender, channel_name);
		else
		{
			std::string reply = channel_name + " :" + channel->getTopic();
			_replier.sendRPL(TOPIC_SET, sender, reply);
		}
	}
}

void Irc::setOperatorHost(const std::string& hostname)
{
	if (containsForbiddenChars(hostname, " 	\r\n")) //could just use isValidPassword() from main?
		return;
	_op_host = hostname;
}

void Irc::setOperatorPW(const std::string& password)
{
	if (containsForbiddenChars(password, " 	\r\n")) //could just use isValidPassword() from main?
		return;
	_op_password = password; 
}
