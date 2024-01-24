#include "../Includes/Irc.hpp"


//con- and destructer
Irc::Irc(): AServer() {}
Irc::Irc(std::string password): AServer(password) {}
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
	else if (cmd == "PASS") PASS(sender, sstream, new_client_fd); //we only take the last PASS // client can always try PASS although not registered ?
	else if (cmd == "NICK") NICK(sender, sstream);
	else if (cmd == "USER")	USER(sender, sstream);
	else if (sender->isRegistered() == false) sendError(ERR_NOTREGISTERED, sender, ""); //?
	else if (cmd == "PRIVMSG") PRIVMSG(sender, sstream);
	else if (cmd == "JOIN") JOIN(sender, sstream);
	else if (cmd == "PART") PART(sender, sstream);
	else if (cmd == "QUIT") QUIT(sender, sstream);
	else if (cmd == "KICK") std::cout << "KICK()" << std::endl; //KICK();
	else if (cmd == "INVITE") std::cout << "INVITE()" << std::endl; //INVITE();
	else if (cmd == "MODE") std::cout << "MODE()" << std::endl; //MODE();
	else if (cmd == "TOPIC") std::cout << "TOPIC()" << std::endl; //TOPIC();
	else if (cmd == "OPER") OPER(sender, sstream);
	else sendError(ERR_UNKNOWNCOMMAND, sender, cmd);
	std::cout << std::endl;
}

//methods (commands)
int	Irc::JOIN(Client *sender, std::stringstream &sstream)
{
	channel_map_iter_t channel_itr;
	std::string	channel_name = extractWord(sstream);

	if(!(channel_name[0] == '#' || channel_name[0] == '&') || channel_name.size() > 200) //check if channel_name is valid
		return (sendError(ERR_NOSUCHCHANNEL, sender, channel_name));
	channel_itr = _channels.find(channel_name);
	if(channel_itr == _channels.end()) // create if channel non exist ?
		addNewChannelToMap(sender, channel_name);
	else
	{
		channel_itr->second->addClient(sender, false);
		//if error send error msg to client ?
		//if( != 0) ?
		//	return ; ?
	}
	channel_itr = _channels.find(channel_name);
	sender->joinChannel(channel_itr->second);
	sendRPL(RPL_JOIN, sender, channel_name);
	return (0);
}


//---------------------------------COMMANDS--------------------------------------------
void	Irc::PART(Client *sender, std::stringstream &sstream)
{
	channel_map_iter_t	channel_it;
	std::stringstream	channel_name_sstream(extractWord(sstream));
	std::string			channel_name;
	std::string			part_msg;
	int					err;

	for (int cnt = 0; cnt < 15 && std::getline(channel_name_sstream, channel_name, ','); cnt++) //std::getline returns eof when stream is empty?
	{
		channel_it = _channels.find(channel_name);
		if (channel_it == _channels.end() || channel_it->second == NULL)
		{
			std::cout << "*Error: PART(): channel is not in channel map" << std::endl;
			sendError(ERR_NOSUCHCHANNEL, sender, channel_name);
			continue ;
		}
		err = channel_it->second->rmClient(sender);
		if (err > 0)
		{
			std::cout << "*Error: PART(): err > 0" << std::endl;
			sendError(static_cast<IRC_ERR>(err), sender, channel_name);
			continue ;
		}
		sender->leaveChannel(channel_it->second);

		part_msg = ":" + sender->getPrefix() + " PART " + channel_name + " :leaving\r\n";
		send(sender->getFd(), part_msg.c_str(), part_msg.size(), 0);
		if (err == -1) //delete channel when empty()
			rmChannelFromMap(channel_name);
	}
	if (!channel_name_sstream.eof())
		return ;
		//sendError(too many argument in list);
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
	int err;
	if (msg.empty())
		msg = "disconnected"; //where will the entire message be concatinated? in channel?
	
	std::vector<Channel *> channels = sender->getAllChannels();
	for (std::vector<Channel *>::iterator it = channels.begin(); it != channels.end(); it++)
	{
		Channel *channel = (*it); 
		if (!channel) //necessary? checking if the channel is in map or null seems overkill, since this case should never happen
			continue ;
		// sender->leaveChannel(channel); //unecessary ?? he will leave entire server
		channel_name = channel->getName();
		err = channel->rmClient(sender);
		if (err == -1) //exchange -1 with CHANNEL_DEAD, and use rmClient with a message
			rmChannelFromMap(channel_name);		
	}
	rmClientFromMaps(sender);
}


// void	Irc::KICK(Client *sender, std::stringstream &sstream);
//
//}


void Irc::PASS(Client *sender, std::stringstream &sstream, const int& new_client_fd)
{
	(void)new_client_fd;
    std::string password = extractWord(sstream);
    	
    if (sender->isRegistered()) 
        sendError(ERR_ALREADYREGISTERED, sender, ""); //already registered
    else if (password == _password)
	{
		sender->authenticate();
		/* command_switch(sender, sstream.str() ,new_client_fd);
		command_switch(sender, sstream.str() ,new_client_fd); */
	}
	else
	{
		std::cout << "pw = " << password << std::endl;
		sender->deauthenticate();
		sendError(ERR_PASSWDMISMATCH, sender, "");
		//delete Client and the entry from the map if Pw is wrong? --> multiple PASS not possible then
	}
	return ;
}

void Irc::NICK(Client *sender, std::stringstream &sstream)
{
    std::string nickname = extractWord(sstream);
    //if (sender->isAuthenticated() == false) //didnt do PASS before NICK 
    //    sendError(321, sender) and return; //theres no err_code for it

	client_name_map_iter_t it = _client_names.find(nickname); //key may not be used cuz it creates an entry --> actually good for us no?
	if (it == _client_names.end()) //no one has the nickname
	{
		if (sender->setNickname(nickname) != 0)
		{
			sendError(ERR_ERRONEUSNICKNAME, sender, "");
			return ;
		}
	}
	else
	{
		sendError(ERR_NICKNAMEINUSE, sender, "");
		return ;
	}
	if (sender->isRegistered())
	{
		addClientToNameMap(sender->getNickname(), sender->getFd());
		sendRPL(RPL_WELCOME, sender, sender->getUsername());
	}
}

void	Irc::USER(Client *sender, std::stringstream &sstream)
{
    std::vector<std::string> info(4);

	if (sender->isRegistered())
	{
		sendError(ERR_ALREADYREGISTERED, sender, "");
		return ;
	}

	//for loop a lil weird but fine for now
    for (std::vector<std::string>::iterator it = info.begin(); it != info.end(); it++)
		*it = extractWord(sstream);
    if (sender->setUser(info[0], info[1], info[2], info[3]) == ERR_NEEDMOREPARAMS)
		sendError(ERR_NEEDMOREPARAMS, sender, "");
	if (sender->isRegistered())
	{
		addClientToNameMap(sender->getNickname(), sender->getFd());
		sendRPL(RPL_WELCOME, sender, sender->getUsername());
	}
}



// ERR_NORECIPIENT
// ERR_NOTEXTTOSEND 412
// ERR_NOSUCHNICK
// ERR_CANNOTSENDTOCHAN 404 --> unecessary, mode n,m, and v not required
// ERR_TOOMANYTARGETS ?? I DONT WANT TO COVER THAT
void Irc::PRIVMSG(Client *sender, std::stringstream &sstream)
{
	std::string recipient = extractWord(sstream);
	std::string message = extractWord(sstream);

	message = ":" + sender->getPrefix() + " PRIVMSG " + recipient + " :" + message; //PART uses same method -> extra function?

	if (recipient.empty()) 
		sendError(ERR_NORECIPIENT, sender, ""); //need more params
	else if (message.empty())
		sendError(ERR_NOTEXTTOSEND, sender, ""); //need more params
	else if (recipient.at(0) == '#')
	{
		channel_map_iter_t rec_it = _channels.find(recipient);
		if (rec_it == _channels.end())
			sendError(ERR_NOSUCHCHANNEL, sender, "");
		else
			rec_it->second->sendMsg(sender, message);	
	}
	else if (recipient.find('@') != std::string::npos) //why? so uneccesary 
	{
		std::cout << "";
		//loop through map and check for client prefix, should we implement this even?? --> im against it
		//another map?? or vector with prefix??
	}
	else
	{
		client_name_map_iter_t rec_it = _client_names.find(recipient);
		if (rec_it == _client_names.end())
			sendError(ERR_NOSUCHNICK, sender, ""); //NO SUCH NICK
		else
			sender->sendTo(message, rec_it->second); //should be done by the server??!!!
	}
}

//void Irc::MODE(Client *sender, std::stringstream &sstream);
//void Irc::TOPIC(Client *sender, std::stringstream &sstream);
//void Irc::INVITE(Client *sender, std::stringstream &sstream);

/* void Irc::clientDied(int client_fd) //pure virtual or put this as a normal function in AServer
{
	std::string	channel_name;
	int err;
	Client *client = _client_fds.find(client_fd)->second;

	std::vector<Channel *> channels = client->getAllChannels();
	for (std::vector<Channel *>::iterator it = channels.begin(); it != channels.end(); it++)
	{
		Channel *channel = (*it); 
		if (!channel)
			continue ;
		channel_name = channel->getName();
		err = channel->rmClient(client); //"lost connection" 
		if (err == -1)
			rmChannelFromMap(channel_name);		
	}
	rmClientFromMaps(client);
} */


void Irc::OPER(Client *sender, std::stringstream &sstream)
{
	std::cout << "Executing OPER()" << std::endl;
	std::string	host = extractWord(sstream); //chose to name the string "host" and not "user" irc protocoll a bit vague
	std::string	pw = extractWord(sstream);
	
	if (host.empty() || pw.empty())
		sendError(ERR_NEEDMOREPARAMS, sender, "");
	else if (host != "_op_host" && sender->getHost() != "_op_host") //need an attribute in AServer or IRC!
		sendError(ERR_NOOPERHOST, sender, "");
	else if (pw != "_op_pw") //need an attribute in AServer or IRC!
		sendError(ERR_PASSWDMISMATCH, sender, "");
	else
	{
		sender->elevateToServOp(); //what if send fails?
		sendRPL(RPL_YOUREOPER, sender, "");
	}
}