#include "../Includes/Irc.hpp"


//con- and destructer
Irc::Irc(const std::string& name, const std::string& password): 
AServer(name, password),
_replier(_name),
_op_host("OpHost"),
_op_password("OpPass")
{}
/* Irc::Irc(const Irc& I);
Irc::Irc operator=(const Irc& I); */
Irc::~Irc() {}

//private methods 

void	Irc::command_switch(Client *sender, const std::string message) //message-> 'request' better name? for us to discern
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
			std::cout << "YOU are a imposter" << std::endl;
			//send()
			return;
		}
		std::getline(sstream, cmd, ' '); //extractWord?
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
	else if (cmd == "INVITE") INVITE(sender, sstream);
	else if (cmd == "MODE") MODE(sender, sstream);
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
		sender->deauthenticate();
		_replier.sendError(ERR_PASSWDMISMATCH, sender, "");
		disconnectClient(sender, "Wrong password"); //delete Client and the entry from the map if Pw is wrong? --> multiple PASS not possible then
		return ;
	}
	if (sender->isRegistered())
		_replier.sendRPL(RPL_WELCOME, sender, sender->getUsername());
}

void Irc::NICK(Client *sender, std::stringstream &sstream)
{
    std::string nickname = extractWord(sstream);

	if (nickname.empty())
		_replier.sendError(ERR_NONICKNAMEGIVEN, sender, "");
	else if (getClient(nickname) == NULL) //no one has the nickname
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
	addClientToNameMap(sender->getNickname(), sender->getFd());
	if (sender->isRegistered())
		_replier.sendRPL(RPL_WELCOME, sender, sender->getUsername());
}

void	Irc::USER(Client *sender, std::stringstream &sstream)
{
    std::vector<std::string> info(4);
	//if (sender->isAuthenticated() == false) //didnt do PASS before
    //   return  _replier.sendError(321, sender) and return; //NOTICE message? sendNotice?
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
		_replier.sendRPL(RPL_WELCOME, sender, sender->getUsername());
}

bool Irc::isChannelNameValid(const std::string &channel_name)
{
	if(!(channel_name[0] == '#' || channel_name[0] == '&') || channel_name.size() > 200)
		return (false);
	for(int i = 0; channel_name[i]; i++)
		if(channel_name[i] == ' ' || channel_name[i] == ',' || channel_name[i] == '\a')
			return (false);
	return (true);
}


int	Irc::JOIN(Client *sender, std::stringstream &sstream)
{
	std::stringstream stream_name(extractWord(sstream)), stream_key(extractWord(sstream));
	std::string	channel_key, channel_name;
	int cnt = 0, err = 0;
	Channel *channel;

	while(cnt < LIST_LIMIT && getline(stream_name, channel_name, ','))
	{
		cnt++;
		getline(stream_key, channel_key, ',');
		if (isChannelNameValid(channel_name) == false) //check if channel_name is valid
		{
			_replier.sendError(ERR_NOSUCHCHANNEL, sender, channel_name);
			continue;
		}
		if (sender->spaceForChannel() == false)
		{
			_replier.sendError(ERR_TOOMANYCHANNELS, sender, channel_name);
			continue;
		}
		channel = getChannel(channel_name);
		if(channel == NULL) // create if channel not exist
			addNewChannelToMap(sender, channel_name);
		else
		{
			err = channel->addClient(sender, channel_key, false);
			if(err > 0)
			{
				_replier.sendError(static_cast<IRC_ERR>(err), sender, channel_name);
				continue;
			}
			if(err < 0)
				continue;
		}
		channel = getChannel(channel_name);
		sender->joinChannel(channel);
		channel->sendMsg(NULL, ":" + sender->getPrefix() + " JOIN " + channel_name + " * :" + sender->getUsername() + "\r\n");
	}
	if (cnt == 0)
		_replier.sendError(ERR_NEEDMOREPARAMS, sender, "");
	return (0);
}

int	Irc::PART(Client *sender, std::stringstream &sstream)
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
			_replier.sendError(ERR_NOSUCHCHANNEL, sender, channel_name);
			continue ;
		}
		part_msg = createMsg(sender, "PART", channel_name, "Leaving");
		err = channel->rmClient(sender, part_msg);
		if (err > 0)
		{
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
		return -1; //_replier.sendError(too many argument in list)!
	return (0);
}

//cant use PART
// - doesnt support listing with ',' e.g: "hungry,bye guys"
// - i dont write myself a message when quiting
// - only errors with quit would be if client is the last in channel
// 		or if the channel doesnt exist
void	Irc::QUIT(Client *sender, std::stringstream &sstream)
{
	std::string	comment = extractWord(sstream);
	if (comment.empty())
		comment = "Leaving";
	disconnectClient(sender, createMsg(sender, "QUIT", "", comment)); //?? what if he had another cmd after that our program would fail
}

int	Irc::KICK(Client *sender, std::stringstream &sstream)
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
		return (_replier.sendError(ERR_NEEDMOREPARAMS, sender, "KICK"));
	channel = getChannel(channel_name);
	if (channel == NULL)
		return (_replier.sendError(ERR_NOSUCHCHANNEL, sender, channel_name));
	user_to_kick = getClient(nickname);
	if (user_to_kick == NULL)
		return (_replier.sendError(ERR_NOSUCHNICK, sender, nickname)); //ERR-NOSUCHNICK is not in list of numeric replies for kick in protocoll

	msg = ":" + sender->getPrefix() + " KICK " + channel_name + " " + nickname + " :" + msg + "\r\n";
	err = channel->rmClient(sender, user_to_kick, msg);
	if (err > 0)
		return (_replier.sendError(static_cast<IRC_ERR>(err), sender, channel_name));
	user_to_kick->leaveChannel(channel);
	if (err == -1) //delete channel when empty()
		rmChannelFromMap(channel_name);
	return (0);
}



// ERR_CANNOTSENDTOCHAN 404 --> unecessary, mode n,m, and v not required
// Hexchat doesnt allow PRIVMSG with channels
// our function allows /PRIVMSG nick1,#chan1 :message -> fine for me
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
		{
			_replier.sendError(ERR_NORECIPIENT, sender, "PRIVMSG");
			continue; 
		}
		else if (recipient.at(0) == '#')
		{
			Channel *channel = getChannel(recipient);
			if (channel == NULL)
				_replier.sendError(ERR_NOSUCHCHANNEL, sender, recipient);
			else if (message.empty())
				_replier.sendError(ERR_NOTEXTTOSEND, sender, ""); //return message is checked before to avoid checking in loop
			else
				channel->sendMsg(sender, createMsg(sender, "PRIVMSG", recipient, message));
		} //mask?
		else
		{
			Client *reciever = getClient(recipient);
			if (reciever == NULL)
				_replier.sendError(ERR_NOSUCHNICK, sender, "");
			else if (message.empty())
				_replier.sendError(ERR_NOTEXTTOSEND, sender, ""); //return message is checked before to avoid checking in loop
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

std::string Irc::createMsg(Client *sender, const std::string& cmd, const std::string& recipient, const std::string& msg) const
{
    std::string message;
	if (!sender)
        return msg;
	if (recipient.empty())
		message = ":" + sender->getPrefix() + " " + cmd + " :" + msg + "\r\n";
    else
		message = ":" + sender->getPrefix() + " " + cmd + " " + recipient + " :" + msg + "\r\n"; //PART uses same method -> extra function?
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
	if (topic.empty()) //only wants to see current topic
	{
		if (channel->getTopic().empty())
			_replier.sendRPL(RPL_NOTOPIC, sender, channel_name);
		else
		{
			std::string reply = channel_name + " :" + channel->getTopic();
			_replier.sendRPL(RPL_TOPIC, sender, reply);
		}
	}
	else
	{
		int err = channel->setTopic(sender->getNickname(), topic); //checks if sender is operator??
		if (err != 0)
			_replier.sendError(static_cast<IRC_ERR>(err), sender, channel_name);
		else
		{
			std::string reply = channel_name + " :" + channel->getTopic();
			_replier.sendRPL(TOPIC_SET, sender, reply);
		}
	}
}

//RPL_INVITING
//ERR_NOSUCHNICK
//ERR_NOTONCHANNEL
//ERR_USERONCHANNEL
//ERR_NEEDMOREPARAMS
//ERR_CHANOPRIVSNEEDED
int	Irc::INVITE(Client *sender, std::stringstream& sstream)
{	
	Channel		*channel;
	Client		*user_to_invite;
	std::string	nickname(extractWord(sstream));
	std::string	channel_name(extractWord(sstream));

	user_to_invite = getClient(nickname);
	channel = getChannel(channel_name);
	if (nickname.empty() || channel_name.empty())		//check if enough params
		return (_replier.sendError(ERR_NEEDMOREPARAMS, sender, "INVITE")); //INVITE?
	if (user_to_invite == NULL)							//check if nick exists
		return (_replier.sendError(ERR_NOSUCHNICK, sender, nickname));
	if (channel == NULL)								//check if channel exists
		return (_replier.sendError(ERR_NOSUCHCHANNEL, sender, channel_name));
	if (!channel->isInChannel(sender))					//check if sender is on channel
		return (_replier.sendError(ERR_NOTONCHANNEL, sender, channel_name));
	if (!channel->isOperator(sender))					//check if sender is operator
		return (_replier.sendError(ERR_CHANOPRIVSNEEDED, sender, channel_name));
	if (channel->isInChannel(user_to_invite))			//check if nick is on channel
		return (_replier.sendError(ERR_USERONCHANNEL, sender, nickname + " " + channel_name));

	channel->addInvited(user_to_invite);
	protectedSend(user_to_invite->getFd(), ":" + sender->getPrefix() + " INVITE " + nickname + " " + channel_name);
	return (0);
}


//chose to name the string "host" and not "user" irc protocoll a bit vague
void Irc::OPER(Client *sender, std::stringstream &sstream)
{
	std::string	host = extractWord(sstream); 
	std::string	pw = extractWord(sstream);
	
	if (host.empty() || pw.empty())
		_replier.sendError(ERR_NEEDMOREPARAMS, sender, "");
	else if (host != _op_host || sender->getHost() != _op_host) //u have to pass the correct host and u need to have that host-ip yourself ???
		_replier.sendError(ERR_NOOPERHOST, sender, "");
	else if (pw != _op_password)
		_replier.sendError(ERR_PASSWDMISMATCH, sender, "");
	else
	{
		sender->elevateToServOp(); //what if send in next line fails?
		_replier.sendRPL(RPL_YOUREOPER, sender, "");
		std::cout << "INFO: " << sender->getPrefix() << " is now server op, chaos is coming!" << std::endl; //out?
	}
}

//483 ERR_CANTKILLSERVER; how do u even trigger this? not covered
int Irc::KILL(Client *sender, std::stringstream &sstream)
{
	std::string nickname;
	std::string comment; 
	Client*  client_to_kill;

	if (sender->isServerOp() == false)
		return (_replier.sendError(ERR_NOPRIVILEGES, sender, nickname));

	nickname = extractWord(sstream);
	if (nickname.empty())
		return (_replier.sendError(ERR_NEEDMOREPARAMS, sender, nickname));
	
	client_to_kill = getClient(nickname);
	if (client_to_kill == NULL)
		return (_replier.sendError(ERR_NOSUCHNICK, sender, nickname));
	
	comment = extractWord(sstream);
	disconnectClient(client_to_kill, comment); //what about killing himself? and another command after that??
	return (0);
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

//returns 403 no such channel, returns 442 not on channel
int Irc::MODE(Client *sender, std::stringstream &sstream)
{
    std::map< std::string, int> operator_rpl_map;
	std::string channel_name, word;
	std::vector<int> error_vec(3);
	Channel *channel;
    char pre_fix;

    channel_name = extractWord(sstream);
	channel = getChannel(channel_name);
	error_vec[0] = -1;
	error_vec[1] = -2;
	error_vec[2] = -3;
	if(channel == NULL)
		return(_replier.sendError(ERR_NOSUCHCHANNEL, sender, channel_name));
	if(channel->isInChannel(sender) == false)
		return(_replier.sendError(ERR_NOTONCHANNEL, sender, channel_name));
	if(channel->isOperator(sender) == false)
		return(_replier.sendError(ERR_CHANOPRIVSNEEDED, sender, channel_name));
	word = extractWord(sstream);
	if(word.empty() == true)
	{
		modesAreSetTo(sender, channel);
		return 1;
	}
	while(word.empty() == false)
    {
        pre_fix = '+';
        for(size_t i = 0; i < word.size(); i++)
        {
			if(channel->isOperator(sender) == false)
				break ;
            if(word[i] == '+' || word[i] == '-')
			{
                pre_fix = word[i];
				i++;
			}
			modesSwitch(channel, sender, operator_rpl_map, sstream, pre_fix, error_vec, word[i]);
        }
		word = extractWord(sstream);
    }
	operatorsSendSetModeToChannel(channel, sender, operator_rpl_map);
	sendSetModeToChannel(channel, sender, error_vec[0], error_vec[1]);
	return (0);
}

void Irc::modesAreSetTo(Client *sender, Channel *channel)
{
	std::stringstream sstream;
	std::string msg(channel->getName() + " +");
	std::string args;

	if(channel->getRestrictTopic() == true)
		msg += "t";
	if(channel->getInviteOnly() == true)
		msg += "i";
	if(channel->getMaxClients() < MAX_CLIENTS)
	{
		msg += "l";
		sstream << channel->getMaxClients();
		args = " " + sstream.str();
	}
	if(channel->getPassword().empty() == true)
	{
		msg += "k";
		args = " " + channel->getPassword();
	}
	if(msg == channel->getName() + " +")
		msg.clear();
	_replier.sendRPL(RPL_CHANNELMODEIS, sender, msg + args);
}

void Irc::modesSwitch(Channel *channel, Client *sender, std::map<std::string, int> &operator_rpl_map, std::stringstream &sstream, const char &pre_fix, std::vector<int> &error_codes, const char &word_char)
{
	int 		key_code = -1;
	std::string argument;

	if(word_char == 'i')
		error_codes[0] = channel->setTopicOrInv(pre_fix, word_char);
	else if(word_char == 't')
		error_codes[1] = channel->setTopicOrInv(pre_fix, word_char);
	else if(word_char == 'l' )
	{
		if(pre_fix == '+')
			argument = extractWord(sstream);
		if(error_codes[2] == MODE_SET_PLUS || error_codes[2] == MODE_SET_MINUS)
			return ;
		error_codes[2] = channel->setMaxClients(argument, pre_fix);
		if(error_codes[2] == MODE_SET_PLUS || error_codes[2] == MODE_SET_MINUS)
			channel->sendMsg(sender, ":" + sender->getPrefix() + " MODE " + channel->getName() + " " + pre_fix + "k " + argument + "\r\n");
		else if(error_codes[2] == ERR_NEEDMOREPARAMS)
			_replier.sendError(ERR_NEEDMOREPARAMS, sender, "MODE");
	}
	else if(word_char == 'o')
	{
		argument = extractWord(sstream);
		operator_rpl_map[argument] = channel->setOperator(pre_fix, getClient(argument));
	}
	else if(word_char == 'k') // k is deferent triggers error?
	{
		argument = extractWord(sstream);
		if(key_code > 0)
			return ;
		key_code = channel->setPassword(argument, pre_fix);
		if(key_code == MODE_SET_PLUS || key_code == MODE_SET_MINUS)
			channel->sendMsg(sender, ":" + sender->getPrefix() + " MODE " + channel->getName() + " " + pre_fix + "k " + argument + "\r\n");
		if(key_code == ERR_KEYSET)
			_replier.sendError(ERR_KEYSET, sender, argument);
		if(key_code == ERR_NEEDMOREPARAMS)
			_replier.sendError(ERR_NEEDMOREPARAMS, sender, "MODE");
	}
	else
		_replier.sendError(ERR_UNKNOWNMODE, sender, argument = word_char);
}

void Irc::operatorsSendSetModeToChannel(Channel *channel, Client *sender, const std::map<std::string, int> &operator_rpl_map)
{
	std::string o_modes_str, o_set_names;
	IRC_ERR error_code;

	for (std::map< std::string, int>::const_iterator o_itr = operator_rpl_map.begin(); o_itr != operator_rpl_map.end(); o_itr++)
	{
		error_code = static_cast<IRC_ERR>(o_itr->second);
		if (error_code == MODE_SET_PLUS)
		{
			o_modes_str += "+o";
			o_set_names += " " + o_itr->first;
		}
		else if (error_code == MODE_SET_MINUS)
		{
			o_modes_str += "-o";
			o_set_names += " " + o_itr->first;
		}
		else if (error_code == ERR_USERNOTINCHANNEL)
			_replier.sendError(error_code, sender, o_itr->first + " " + channel->getName());
		else if (error_code == ERR_NOSUCHNICK)
			_replier.sendError(error_code, sender, o_itr->first);
	}
	if(o_modes_str.empty() == false)
		channel->sendMsg(sender, ":" + sender->getPrefix() + " MODE " + channel->getName() + " " + o_modes_str + o_set_names + "\r\n");
}

void Irc::sendSetModeToChannel(Channel *channel, Client *sender, const int &inv_code, const int &topic_code)
{
	if(inv_code == MODE_SET_PLUS)
		channel->sendMsg(NULL, ":" + sender->getPrefix() + " MODE " + channel->getName() + " +i\r\n");
	else if(inv_code == MODE_SET_MINUS)
		channel->sendMsg(NULL, ":" + sender->getPrefix() + " MODE " + channel->getName() + " -i\r\n");
	if(topic_code == MODE_SET_PLUS)
		channel->sendMsg(NULL, ":" + sender->getPrefix() + " MODE " + channel->getName() + " +t\r\n");
	else if(topic_code == MODE_SET_MINUS)
		channel->sendMsg(NULL, ":" + sender->getPrefix() + " MODE " + channel->getName() + " -t\r\n");
}
