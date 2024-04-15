#include "../Includes/Irc.hpp"


//con- and destructer
Irc::Irc(const std::string& name, const std::string& password): 
AServer(name, password),
_replier(_name),
_sender(NULL),
_op_host("OpHost"),
_op_password("OpPass")
{}
/* Irc::Irc(const Irc& I);
Irc::Irc operator=(const Irc& I); */
Irc::~Irc() {}

//private methods
void	Irc::command_switch(Client *sender, const std::string message)
{
	_sender = sender;
	// std::cout << "msg from fd" << sender->getFd() << ": " << message << "!" << std::endl;

    std::stringstream	sstream(message);
    std::string	cmd;

	//this block checks for Prefix 
	std::getline(sstream >> std::ws, cmd, ' ');
	if (cmd.empty())
		return ;
	if (cmd.at(0) == ':')
	{
		if (cmd != ":" + sender->getPrefix())
			return;
		std::getline(sstream, cmd, ' ');
	}

	if (cmd == "CAP")
		return ;
	else if (cmd == "PASS") PASS(sstream);
	else if (cmd == "NICK") NICK(sstream);
	else if (cmd == "USER")	USER(sstream);
	else if (_sender->isRegistered() == false) _replier.sendError(ERR_NOTREGISTERED, _sender, "");
	else if (cmd == "PRIVMSG") PRIVMSG(sstream);
	else if (cmd == "JOIN") JOIN(sstream);
	else if (cmd == "PART") PART(sstream);
	else if (cmd == "QUIT") QUIT(sstream);
	else if (cmd == "KICK") KICK(sstream);
	else if (cmd == "WHO") WHO(sstream);
	else if (cmd == "INVITE") INVITE(sstream);
	else if (cmd == "MODE") MODE(sstream);
	else if (cmd == "TOPIC") TOPIC(sstream);
	else if (cmd == "OPER") OPER(sstream);
	else if (cmd == "KILL") KILL(sstream);
	else _replier.sendError(ERR_UNKNOWNCOMMAND, _sender, cmd);
	/* std::cout << std::endl; */
}

//methods (commands)
//---------------------------------COMMANDS--------------------------------------------
void Irc::PASS(std::stringstream &sstream)
{
    std::string password = extractWord(sstream);

    if (_sender->isRegistered())
	{
        _replier.sendError(ERR_ALREADYREGISTERED, _sender, ""); //already registered
		return ;
	}
	else if (password.empty())
	{
		_replier.sendError(ERR_NEEDMOREPARAMS, _sender, "PASS"); //already registered
		return ;
	}
    else if (password == _password)
		_sender->authenticate();
	else
	{
		_sender->deauthenticate();
		_replier.sendError(ERR_PASSWDMISMATCH, _sender, "");
		disconnectClient(_sender, "Wrong password");
		return ;
	}
	if (_sender->isRegistered())
	{
		_replier.sendRPL(RPL_WELCOME, _sender, _sender->getUsername());
		_replier.sendRPL(RPL_ISUPPORT, _sender, "");
	}
}

int Irc::NICK(std::stringstream &sstream)
{
    std::string nickname = extractWord(sstream);
	std::string old_nick, old_prefix;

	if (_sender->isAuthenticated() == false)
		return 1;
	if (nickname.empty())
		return (_replier.sendError(ERR_NONICKNAMEGIVEN, _sender, ""));
	if (getClient(nickname) != NULL)
		return (_replier.sendError(ERR_NICKNAMEINUSE, _sender, nickname));
	old_prefix = _sender->getPrefix();
	old_nick = _sender->getNickname();

	if (_sender->setNickname(nickname) != 0)
	{
		/* std::cout << "nickname: " << nickname << std::endl;
		std::cout << "_sender: " << _sender->getNickname() << std::endl; */
		/* 	if (_sender->getNickname().empty())
			sender */
		return (_replier.sendError(ERR_ERRONEUSNICKNAME, _sender, nickname));
	}
	if (old_nick.empty() == false)
	{
		rmClientFromNameMap(old_nick);
		addClientToNameMap(_sender->getNickname(), _sender->getFd());
		if (_sender->isRegistered() == true)
			protectedSend(_sender, ":" + old_prefix + " NICK :" + _sender->getNickname());
		return 2;
	}
	addClientToNameMap(_sender->getNickname(), _sender->getFd());
	if (_sender->isRegistered() == true)
	{
		_replier.sendRPL(RPL_WELCOME, _sender, _sender->getUsername());
		_replier.sendRPL(RPL_ISUPPORT, _sender, "");
	}
	return 0;
}

void	Irc::USER(std::stringstream &sstream)
{
    std::vector<std::string> info(4);

	if (_sender->isAuthenticated() == false)
		return ;
	if (_sender->isRegistered())
	{
		_replier.sendError(ERR_ALREADYREGISTERED, _sender, "");
		return ;
	}
    for (std::vector<std::string>::iterator it = info.begin(); it != info.end(); it++)
		*it = extractWord(sstream);
    if (_sender->setUser(info[0], info[1], info[2], info[3]) == ERR_NEEDMOREPARAMS)
		_replier.sendError(ERR_NEEDMOREPARAMS, _sender, "USER");
	if (_sender->isRegistered() == true)
	{
		_replier.sendRPL(RPL_WELCOME, _sender, _sender->getUsername());
		_replier.sendRPL(RPL_ISUPPORT, _sender, "");
	}
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


int	Irc::JOIN(std::stringstream &sstream)
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
			_replier.sendError(ERR_NOSUCHCHANNEL, _sender, channel_name);
			continue;
		}
		if (_sender->spaceForChannel() == false)
		{
			_replier.sendError(ERR_TOOMANYCHANNELS, _sender, channel_name);
			continue;
		}
		channel = getChannel(channel_name);
		if(channel == NULL) // create if channel not exist
			addNewChannelToMap(_sender, channel_name);
		else
		{
			err = channel->addClient(_sender, channel_key, false);
			if(err > 0)
			{
				_replier.sendError(static_cast<IRC_ERR>(err), _sender, channel_name);
				continue;
			}
			if(err < 0)
				continue;
		}
		channel = getChannel(channel_name);
		_sender->joinChannel(channel);
		channel->sendMsg(NULL, ":" + _sender->getPrefix() + " JOIN " + channel_name + " * :" + _sender->getUsername());
	}
	if (cnt == 0)
		_replier.sendError(ERR_NEEDMOREPARAMS, _sender, "JOIN");
	return (0);
}

int	Irc::PART(std::stringstream &sstream)
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
			_replier.sendError(ERR_NOSUCHCHANNEL, _sender, channel_name);
			continue ;
		}
		part_msg = createMsg(_sender, "PART", channel_name, "Leaving");
		err = channel->rmClient(_sender, part_msg);
		if (err > 0)
		{
			_replier.sendError(static_cast<IRC_ERR>(err), _sender, channel_name);
			continue ;
		}
		_sender->leaveChannel(channel);
		if (err == DELETE_CHANNEL) //delete channel when empty()
			rmChannelFromMap(channel_name);
	}
	if (cnt == 0)
		_replier.sendError(ERR_NEEDMOREPARAMS, _sender, "PART");
	if (!channel_name_sstream.eof())
		return -1; //_replier.sendError(too many argument in list)!
	return (0);
}

//cant use PART
// - doesnt support listing with ',' e.g: "hungry,bye guys"
// - i dont write myself a message when quiting
// - only errors with quit would be if client is the last in channel
// 		or if the channel doesnt exist
void	Irc::QUIT(std::stringstream &sstream)
{
	std::string	comment = extractWord(sstream);
	if (comment.empty())
		comment = "Leaving";
	disconnectClient(_sender, createMsg(_sender, "QUIT", "", comment));
}

int	Irc::KICK(std::stringstream &sstream)
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
		return (_replier.sendError(ERR_NEEDMOREPARAMS, _sender, "KICK"));
	channel = getChannel(channel_name);
	if (channel == NULL)
		return (_replier.sendError(ERR_NOSUCHCHANNEL, _sender, channel_name));
	user_to_kick = getClient(nickname);
	if (user_to_kick == NULL)
		return (_replier.sendError(ERR_NOSUCHNICK, _sender, nickname)); //ERR-NOSUCHNICK is not in list of numeric replies for kick in protocoll

	msg = ":" + _sender->getPrefix() + " KICK " + channel_name + " " + nickname + " :" + msg;
	err = channel->rmClient(_sender, user_to_kick, msg);
	if (err > 0)
		return (_replier.sendError(static_cast<IRC_ERR>(err), _sender, channel_name));
	user_to_kick->leaveChannel(channel);
	if (err == -1) //delete channel when empty()
		rmChannelFromMap(channel_name);
	return (0);
}

int	Irc::WHO(std::stringstream& sstream)
{
	Channel		*channel;
	std::string	channel_name = extractWord(sstream);

	channel = getChannel(channel_name);
	if (channel == NULL)
		return 1;
	if (channel->isInChannel(_sender) == false)
		return 2;
	channel->sendWhoMessage(_sender, _name);
	return 0;
}



// ERR_CANNOTSENDTOCHAN 404 --> unecessary, mode n,m, and v not required
// Hexchat doesnt allow PRIVMSG with channels
// our function allows /PRIVMSG nick1,#chan1 :message -> fine for me
void Irc::PRIVMSG(std::stringstream &sstream)
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
			_replier.sendError(ERR_NORECIPIENT, _sender, "PRIVMSG");
			continue; 
		}
		else if (recipient.at(0) == '#' || recipient.at(0) == '&')
		{
			Channel *channel = getChannel(recipient);
			if (channel == NULL)
				_replier.sendError(ERR_NOSUCHCHANNEL, _sender, recipient);
			else if (channel->isInChannel(_sender) == false)
				_replier.sendError(ERR_CANNOTSENDTOCHAN, _sender, recipient);
			else if (message.empty())
				_replier.sendError(ERR_NOTEXTTOSEND, _sender, ""); //return message is checked before to avoid checking in loop
			else
				channel->sendMsg(_sender, createMsg(_sender, "PRIVMSG", recipient, message));
		}
		else
		{
			Client *reciever = getClient(recipient);
			if (reciever == NULL)
				_replier.sendError(ERR_NOSUCHNICK, _sender, recipient);
			else if (message.empty())
				_replier.sendError(ERR_NOTEXTTOSEND, _sender, ""); //return message is checked before to avoid checking in loop
			else
			{
				reply = createMsg(_sender, "PRIVMSG", recipient, message);
				protectedSend(reciever, reply);
			}
		}
	}
	if (cnt == 0)
		_replier.sendError(ERR_NORECIPIENT, _sender, "PRIVMSG");
}

std::string Irc::createMsg(Client *sender, const std::string& cmd, const std::string& recipient, const std::string& msg) const
{
    std::string message;
	if (!sender)
        return msg;
	if (recipient.empty())
		message = ":" + sender->getPrefix() + " " + cmd + " :" + msg;
    else
		message = ":" + sender->getPrefix() + " " + cmd + " " + recipient + " :" + msg;
    return message;
}


void Irc::TOPIC(std::stringstream &sstream)
{
	std::string channel_name = extractWord(sstream);
	std::string topic = extractWord(sstream);
	Channel* 	channel;

	if (channel_name.empty())
		return (_replier.sendError(ERR_NEEDMOREPARAMS, _sender, "TOPIC"), void());
	channel = getChannel(channel_name);
	if (channel == NULL)
		return (_replier.sendError(ERR_NOSUCHCHANNEL, _sender, channel_name), void());
	else if (channel->isInChannel(_sender) == false)
		return (_replier.sendError(ERR_NOTONCHANNEL, _sender, channel_name), void());
	else if (topic.empty()) //only wants to see current topic
	{
		if (channel->getTopic().empty())
			_replier.sendRPL(RPL_NOTOPIC, _sender, channel_name);
		else
			_replier.sendRPL(RPL_TOPIC, _sender, _sender->getNickname() + " " + channel_name + " :" + channel->getTopic());
	}
	else
	{
		int err = channel->setTopic(_sender->getNickname(), topic); //checks if sender is operator
		if (err != TOPIC_SET)
			_replier.sendError(static_cast<IRC_ERR>(err), _sender, channel_name);
		else
			channel->sendMsg(NULL, ":" + _sender->getPrefix() + " TOPIC " + channel_name + " :" + channel->getTopic());
	}
}

//RPL_INVITING
//ERR_NOSUCHNICK
//ERR_NOTONCHANNEL
//ERR_USERONCHANNEL
//ERR_NEEDMOREPARAMS
//ERR_CHANOPRIVSNEEDED
int	Irc::INVITE(std::stringstream& sstream)
{	
	Channel		*channel;
	Client		*user_to_invite;
	std::string	nickname(extractWord(sstream));
	std::string	channel_name(extractWord(sstream));

	user_to_invite = getClient(nickname);
	channel = getChannel(channel_name);
	if (nickname.empty() || channel_name.empty())		//check if enough params
		return (_replier.sendError(ERR_NEEDMOREPARAMS, _sender, "INVITE"));
	if (user_to_invite == NULL)							//check if nick exists
		return (_replier.sendError(ERR_NOSUCHNICK, _sender, nickname));
	if (channel == NULL)								//check if channel exists
		return (_replier.sendError(ERR_NOSUCHCHANNEL, _sender, channel_name));
	if (!channel->isInChannel(_sender))					//check if sender is on channel
		return (_replier.sendError(ERR_NOTONCHANNEL, _sender, channel_name));
	if (!channel->isOperator(_sender))					//check if sender is operator
		return (_replier.sendError(ERR_CHANOPRIVSNEEDED, _sender, channel_name));
	if (channel->isInChannel(user_to_invite))			//check if nick is on channel
		return (_replier.sendError(ERR_USERONCHANNEL, _sender, nickname + " " + channel_name));

	channel->addInvited(user_to_invite);
	protectedSend(user_to_invite, ":" + _sender->getPrefix() + " INVITE " + nickname + " " + channel_name);
	_replier.sendRPL(RPL_INVITING, _sender, nickname + " " + channel_name);
	return (0);
}


//chose to name the string "host" and not "user" irc protocoll a bit vague
void Irc::OPER(std::stringstream &sstream)
{
	std::string	host = extractWord(sstream); 
	std::string	pw = extractWord(sstream);
	
	if (host.empty() || pw.empty())
		_replier.sendError(ERR_NEEDMOREPARAMS, _sender, "OPER");
	else if (host != _op_host || _sender->getHost() != _op_host) //u have to pass the correct host and u need to have that host-ip yourself ???
		_replier.sendError(ERR_NOOPERHOST, _sender, "");
	else if (pw != _op_password)
		_replier.sendError(ERR_PASSWDMISMATCH, _sender, "");
	else
	{
		_sender->elevateToServOp(); //what if send in next line fails?
		_replier.sendRPL(RPL_YOUREOPER, _sender, "");
		std::cout << "INFO: " << _sender->getPrefix() << " is now server op, chaos is coming!" << std::endl;
	}
}

int Irc::KILL(std::stringstream &sstream)
{
	std::string nickname;
	std::string comment; 
	Client*  client_to_kill;

	if (_sender->isServerOp() == false)
		return (_replier.sendError(ERR_NOPRIVILEGES, _sender, nickname));

	nickname = extractWord(sstream);
	if (nickname.empty())
		return (_replier.sendError(ERR_NEEDMOREPARAMS, _sender, "KILL"));
	
	client_to_kill = getClient(nickname);
	if (client_to_kill == NULL)
		return (_replier.sendError(ERR_NOSUCHNICK, _sender, nickname));
	
	comment = extractWord(sstream);
	protectedSend(client_to_kill, ":" + _sender->getPrefix() + " KILL " + nickname + " :" + comment);
	setSender(client_to_kill);
	disconnectClient(client_to_kill, createMsg(_sender, "QUIT", "", "i was killed by server operator"));
	return (0);
}

void Irc::setOperatorHost(const std::string& hostname)
{
	if (containsForbiddenChars(hostname, " 	\r\n"))
		return;
	_op_host = hostname;
}

void Irc::setOperatorPW(const std::string& password)
{
	if (containsForbiddenChars(password, " 	\r\n"))
		return;
	_op_password = password;
}

//returns 403 no such channel, returns 442 not on channel
int Irc::MODE(std::stringstream &sstream)
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
		return(_replier.sendError(ERR_NOSUCHCHANNEL, _sender, channel_name));
	if(channel->isInChannel(_sender) == false)
		return(_replier.sendError(ERR_NOTONCHANNEL, _sender, channel_name));
	word = extractWord(sstream);
	if(word.empty() == true)
		return (sendModesStatus(channel));
	if(channel->isOperator(_sender) == false)
		return(_replier.sendError(ERR_CHANOPRIVSNEEDED, _sender, channel_name));
	while(word.empty() == false)
    {
        pre_fix = '+';
        for(size_t i = 0; i < word.size(); i++)
        {
			if(channel->isOperator(_sender) == false)
				break ;
            if(word[i] == '+' || word[i] == '-')
			{
                pre_fix = word[i];
				i++;
			}
			modesSwitch(channel, operator_rpl_map, sstream, pre_fix, error_vec, word[i]);
        }
		word = extractWord(sstream);
    }
	operatorsSendSetModeToChannel(channel, _sender, operator_rpl_map);
	sendSetModeToChannel(channel, _sender, error_vec[0], error_vec[1]);
	return (0);
}

int Irc::sendModesStatus(Channel *channel)
{
	std::string			msg(channel->getName() + " +");
	std::stringstream	sstream;
	std::string 		args;

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
	if(channel->getPassword().empty() == false)
	{
		msg += "k";
		args = " " + channel->getPassword();
	}
	if(msg == channel->getName() + " +")
		msg.clear();
	_replier.sendRPL(RPL_CHANNELMODEIS, _sender, msg + args);
	return (0);
}

int Irc::modesSwitch(Channel *channel, std::map<std::string, int> &operator_rpl_map, std::stringstream &sstream, const char &pre_fix, std::vector<int> &error_codes, const char &ch_modes)
{
	int 		key_code = -1;
	std::string argument;
	enum color {SET_RESTRICT_TOPIC = 't', SET_INVITE_ONLY = 'i', SET_LIMIT = 'l', SET_OPERATOR = 'o', SET_KEY = 'k'};

	switch (ch_modes)
	{

		case SET_RESTRICT_TOPIC:
			return (error_codes[1] = channel->setTopicOrInv(pre_fix, ch_modes));

		case SET_INVITE_ONLY:
			return (error_codes[0] = channel->setTopicOrInv(pre_fix, ch_modes));

		case SET_OPERATOR:
			argument = extractWord(sstream);
			if(argument.empty() == false)
				operator_rpl_map[argument] = channel->setOperator(pre_fix, getClient(argument));
			return (1);

		case SET_LIMIT:
			if(pre_fix == '+')
				argument = extractWord(sstream);
			if(error_codes[2] == MODE_SET_PLUS || error_codes[2] == MODE_SET_MINUS)
				return (2);
			error_codes[2] = channel->setMaxClients(argument, pre_fix);
			if(error_codes[2] == MODE_SET_PLUS || error_codes[2] == MODE_SET_MINUS)
				channel->sendMsg(NULL, ":" + _sender->getPrefix() + " MODE " + channel->getName() + " " + pre_fix + "l " + argument);
			else if(error_codes[2] == ERR_NEEDMOREPARAMS)
				_replier.sendError(ERR_NEEDMOREPARAMS, _sender, "MODE");
			return (3);

		case SET_KEY:
			argument = extractWord(sstream);
			if(key_code > 0)
				return (4);
			key_code = channel->setPassword(argument, pre_fix);
			return (sendKeyRPLorError(channel, argument, key_code, pre_fix));

		default:
			_replier.sendError(ERR_UNKNOWNMODE, _sender, argument = ch_modes);

	}
	return (0);
}

int Irc::sendKeyRPLorError(Channel *channel, std::string &argument, const int &key_code, const char &pre_fix)
{
	if(key_code == MODE_SET_PLUS || key_code == MODE_SET_MINUS)
		channel->sendMsg(NULL, ":" + _sender->getPrefix() + " MODE " + channel->getName() + " " + pre_fix + "k " + argument);
	else if(key_code == ERR_KEYSET)
		_replier.sendError(ERR_KEYSET, _sender, argument);
	else if(key_code == ERR_NEEDMOREPARAMS)
		_replier.sendError(ERR_NEEDMOREPARAMS, _sender, "MODE");
	return (5);
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
		channel->sendMsg(NULL, ":" + sender->getPrefix() + " MODE " + channel->getName() + " " + o_modes_str + o_set_names);
}

void	Irc::sendSetModeToChannel(Channel *channel, Client *sender, const int &inv_code, const int &topic_code)
{
	if(inv_code == MODE_SET_PLUS)
		channel->sendMsg(NULL, ":" + sender->getPrefix() + " MODE " + channel->getName() + " +i");
	else if(inv_code == MODE_SET_MINUS)
		channel->sendMsg(NULL, ":" + sender->getPrefix() + " MODE " + channel->getName() + " -i");
	if(topic_code == MODE_SET_PLUS)
		channel->sendMsg(NULL, ":" + sender->getPrefix() + " MODE " + channel->getName() + " +t");
	else if(topic_code == MODE_SET_MINUS)
		channel->sendMsg(NULL, ":" + sender->getPrefix() + " MODE " + channel->getName() + " -t");
}

void	Irc::setSender(Client *sender) { _sender = sender; }

