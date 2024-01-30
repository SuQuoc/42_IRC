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
			std::cout << "YOU are a imposter" << std::endl; //"n!u@" nothing after @ should work; scared bc hexchat has some weird domain after @ :@1321.32133.3213.IRC
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
		sender->deauthenticate();
		_replier.sendError(ERR_PASSWDMISMATCH, sender, "");
		disconnectClient(sender, "Wrong password"); //delete Client and the entry from the map if Pw is wrong? --> multiple PASS not possible then
	}
}

void Irc::NICK(Client *sender, std::stringstream &sstream)
{
    std::string nickname = extractWord(sstream);
    //if (sender->isAuthenticated() == false) //didnt do PASS before
    //    _replier.sendError(321, sender) and return; //NOTICE message? sendNotice?

	client_name_map_const_it it = getClientIter(nickname);
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
	{
		std::cerr << "*Error: PART(): empty sstream" << std::endl;
		_replier.sendError(ERR_NEEDMOREPARAMS, sender, "");
	}
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
	std::string	comment = extractWord(sstream);
	if (comment.empty())
		comment = "Leaving";
	disconnectClient(sender, createMsg(sender, "QUIT", "", comment));
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
		std::cerr << "KICK: > 0" << std::endl;
		_replier.sendError(static_cast<IRC_ERR>(err), sender, channel_name);
		return ;
	}
	user_to_kick->leaveChannel(channel);
	if (err == -1) //delete channel when empty()
		rmChannelFromMap(channel_name);
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

	if (message.empty())
		_replier.sendError(ERR_NOTEXTTOSEND, sender, "");
	while (cnt < LIST_LIMIT && std::getline(recip_sstream, recipient, ','))
	{
		cnt++;
		if (recipient.empty()) 
			_replier.sendError(ERR_NORECIPIENT, sender, "PRIVMSG");
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
	disconnectClient(client_to_kill, comment); 
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
	std::string channel_name, argument, word;
    /* std::map< char, std::pair<char, std::string> > modes_map; */
    std::map< std::string, std::pair<char, int> > o_name_code_map;
	Channel *channel;
    char pre_fix;

    std::getline(sstream >> std::ws, channel_name, ' ');
	channel = getChannel(channel_name); //  test it ????????????????????
	if(channel == NULL)
	{
		_replier.sendError(ERR_NOSUCHCHANNEL, sender, channel_name);
		return ERR_NOSUCHCHANNEL;
	}
	if(channel->isInChannel(sender) == false)					//  test it ????????????????????
	{
		_replier.sendError(ERR_NOTONCHANNEL, sender, channel_name);
		return ERR_NOTONCHANNEL;
	}


	// send just the errors after the loop??????????????
	int inv_code = -42;
	int topic_code = -42;
	int limit_code = -42;
	int operartor_code = -42;	

	while((word = extractWord(sstream)).empty() == false)
    {
        pre_fix = '+';
        for(size_t i = 0; i < word.size(); i++)
        {
			if(channel->isOperator(sender) == false)
				break ;
			argument.clear();
            if(word[i] == '+' || word[i] == '-') // add here for <ws>: ? if we have one just call a funktion that handals this separate
            {
                pre_fix = word[i];
                i++;
            }
            if(word[i] == 'i')
            {
                /* modes_map[word[i]] = std::pair<char,std::string>(pre_fix, ""); */
				inv_code = channel->modesSwitch(sender, pre_fix, word[i], "");
            }
			else if(word[i] == 't')
            {
				topic_code = channel->modesSwitch(sender, pre_fix, word[i], "");
			}
            else if(word[i] == 'l' /* && modes_map.find(word[i]) != modes_map.end() */)
            {
				if(pre_fix == '+')				// does it cut out always need to test ???????????????????????
                	std::getline(sstream >> std::ws, argument, ' ');
				if(limit_code == 324)		// topic was set no changes ?????????????????????????????
					continue ;
				topic_code = channel->modesSwitch(sender, pre_fix, word[i], "");

                /* modes_map[word[i]] = std::pair<char,std::string>(pre_fix, argument); */
            }
            else if(word[i] == 'o')
            {
				std::getline(sstream >> std::ws, argument, ' ');
				operartor_code = channel->modesSwitch(sender, pre_fix, word[i], argument);
				std::cout << operartor_code << std::endl;
				o_name_code_map[argument] = std::pair<char, int>(pre_fix, operartor_code);
                /* mode_o_map[argument] = pre_fix; */
            }
            else if(word[i] == 'k' /* && modes_map.find(word[i]) != modes_map.end() */) // k is deferent triggers error?
            {
                std::getline(sstream >> std::ws, argument, ' ');
                /* channel->modesSwitch(sender, pre_fix, word[i], argument); */
                std::cout << pre_fix << word[i] << " " << argument << std::endl;
            }
        }
    }

	for(std::map< std::string, std::pair<char, int> >::iterator o_itr = o_name_code_map.begin(); o_itr != o_name_code_map.end(); o_itr++)
	{
		if(o_itr->second.second > 0)
			std::cout << o_itr->second.first << o_itr->first << std::endl;
		//_replier.sendError(static_cast<IRC_ERR>(o_itr->second.second), sender, ""); //fix err codes!!
	}
	if(inv_code > 0)
		std::cout << inv_code << std::endl;
		//_replier.sendError(static_cast<IRC_ERR>(o_itr->second.second), sender, ""); //fix err codes!!
	if(topic_code > 0)
		std::cout << topic_code << std::endl;
		//_replier.sendError(static_cast<IRC_ERR>(o_itr->second.second), sender, ""); //fix err codes!!
	if(limit_code > 0)
		std::cout << topic_code << std::endl;
	return (0);


    /* for(std::map<std::string, char>::iterator map_it = mode_o_map.begin(); map_it != mode_o_map.end(); map_it++)
	{
        std::cout << channel->modesSwitch(sender, map_it->second, 'o', map_it->first) << std::endl;
	}

    for(std::map<char, std::pair<char,std::string> >::iterator map_it = modes_map.begin(); map_it != modes_map.end(); map_it++)
    {
		std::cout << channel->modesSwitch(sender, map_it->second.first, map_it->first, map_it->second.second) << std::endl;
	}    
	*/
}
