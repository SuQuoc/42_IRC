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
	// TESTING BLOCK
	// (void)(sender);
	// std::string msg = ":X 401 niki :No such nick/channel\r\n";
	// send(new_client_fd, msg.c_str(), msg.size(), 0);
    std::stringstream	sstream(message); //message can't be empty
    std::string	cmd;
	//this block checks for Prefix 
	std::getline(sstream >> std::ws, cmd, ' ');
	if (cmd.at(0) == ':')
	{
		if (cmd != sender->getPrefix())
		{
			std::cout << "YOU are a imposter" << std::endl; //"n!u@" nothing after @ should work; scared bc hexchat has some weird domain after @ :@1321.32133.3213.IRC
			return;
		}
		std::getline(sstream, cmd, ' ');
	}
	std::cout << "cmd: " << cmd << std::endl;
		
		// std::getline(sstream, cmd); //? for CAP LS 
		// std::getline(sstream, cmd, ' '); //?
		//std::cout << "cmd2: " << cmd << "$" << std::endl;
	if (cmd == "CAP") //we only take the last PASS
		return ;
	else if (cmd == "PASS") //we only take the last PASS
	{
		PASS(sender, sstream, new_client_fd);
		std::cout << "PASS()" << new_client_fd << std::endl; //PASS(new_client_fd); // client can always try PASS although not registered ?
	}
	else if (cmd == "NICK") 
	{
		NICK(sender, sstream);
		std::cout << "NICK() " << sender->getNickname() << std::endl;
	}
	else if (cmd == "USER")
	{
		USER(sender, sstream);
		std::cout << "USER() " << sender->getUsername() << std::endl; 
	}
	else if (sender->isRegistered() == false) sendError(ERR_NOTREGISTERED, sender, ""); //?
	else if (cmd == "PRIVMSG") std::cout << "PRIVMSG()" << std::endl; //PRIVMSG();
	else if (cmd == "JOIN")
	{
		JOIN(sender, sstream);
		std::cout << "JOIN()" << std::endl; //JOIN();
	} 
	else if (cmd == "PART") std::cout << "PART()" << std::endl; //PART();
	else if (cmd == "QUIT") std::cout << "QUIT()" << std::endl; //QUIT();
	else if (cmd == "KICK") std::cout << "KICK()" << std::endl; //KICK();
	else if (cmd == "INVITE") std::cout << "INVITE()" << std::endl; //INVITE();
	else if (cmd == "MODE") std::cout << "MODE()" << std::endl; //MODE();
	else if (cmd == "TOPIC") std::cout << "TOPIC()" << std::endl; //TOPIC();
	else sendError(ERR_UNKNOWNCOMMAND, sender, cmd);
}

std::string	Irc::extractWord(std::stringstream& sstream)
{
	std::string	word;

	sstream >> std::ws;
	if (sstream.peek() == ':')
	{
		sstream.get();
		std::getline(sstream, word);
	}
	else
		std::getline(sstream, word, ' ');
	return (word);
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
		addNewPair(sender, channel_name);
	else
	{
		channel_itr->second->addClient(sender, false);
		//if error send error msg to client ?
		//if( != 0) ?
		//	return ; ?
	}
	sender->joinChannel(channel_itr->second);
	sendRPL(RPL_JOIN, sender, channel_name);
	return (0);
}
// std::string	make_message(sender, msg, default)
// {
// 	std::string	message;

// 	give message sender info
// 	if (msg.empty())
// 		message + default
// 	message + msg;
// 	return (message);
// }

// void	Irc::PART(Client *sender, std::stringstream &sstream)
// {
// 	//PART #channnel-name msg
// 	Channel		*channel;
// 	std::string	channel_name;
// 	std::string	msg;

// 	std::getline(sstream, channel_name);
// 	std::getline(sstream, msg); //make message! //or do it in channel-object?
// 	channel = sender.getChannel(channel_name)
// 	if (channel == std::nullptr)
// 		//send error to sender //or return if send is in client-class
// 	if (channel_map.find(channel_name)->removeClient(sender, msg) == CHANNEL_DEAD); //delete channel when empty()
// 			//get channel_name from channel-vector[i] //not needed here
// 			delete channel through channel-map (channel_name);
// 			erase channel from channel-map
// 	sender->leaveChannel();
// 	channel.sendMsg(sender, make_message(sender, msg, default));
// }
// void	QUIT(Client *sender, std::stringstream &sstream)
// {
// 	std::string	channel_name;
// 	std::string	msg;
// 	std::getline(sstream, msg); //make message! //or do it in channel-object?

// 	get const channel-vector reference from client-map(?)
// 	loop through channel-vector
// 		if (channel-vector[i]->removeClient(sender, msg) == CHANNEL_DEAD); //delete channel when empty()
// 			get channel_name from channel-vector[i]
// 			delete channel through channel-map (channel_name);
// 			erase channel from channel-map
// 		sender->leaveChannel(channel-vector[i]);
// 	erase client from fd_client-map
// 	get client_name (-> channel_name variable?)
// 	delete client-object (sender)
// 	erase client from name_client-map (name)
// }
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

	message = ":" + sender->getPrefix() + "PRIVMSG " + recipient + " :" + message;
	
	// SHOULD WE CHECK FOR:
		// - if recpient == sender.getNickname()? -> u can write yourself a message in hexchat with /PRIVMSG
		// - /PRIVMSG for channels didnt work in Hexchat

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
			sender->sendTo(message, rec_it->second); //should be done by the server??
	}
}


// void Irc::MODE(Client *sender, std::stringstream &sstream);
// void Irc::TOPIC(Client *sender, std::stringstream &sstream);
// void Irc::INVITE(Client *sender, std::stringstream &sstream);
/*
void Irc::PRIVMSG(Client *sender, std::stringstream &sstream);
{
	std::string recipient;
	std::string message;
    std::getline(sstream, recipient, ' '); //space 
    std::getline(sstream, message);


	SHOULD WE CHECK FOR:
		- ':' ?? -> hexchat doesnt let u write /PRIVMSG
		- if recpient == sender.getNickname()? -> u can write yourself a message in hexchat with /PRIVMSG
		- /PRIVMSG for channels didnt work in Hexchat

	if (recipient->empty()) 
		sendError(ERR_); //need more params
	if (recipient[0] == '#')
		_channels.find(recipient)->second->NiksFunctionToRelay(sender, message) 
		//if message is being concatinated in channel it maybe has to know what IRC CMD was triggered
		//then u can use the NiksFunctionToRelay(sender, message) only for one specific IRC CMD and not for e.g. both PRIVMSG and JOIN
	else
		_usersNickname.find(recipient)->second->ClientMemberFunction(message)?
}


void Irc::MODE(Client *sender, std::stringstream &sstream);
void Irc::TOPIC(Client *sender, std::stringstream &sstream);
void Irc::INVITE(Client *sender, std::stringstream &sstream); */

/* void Irc::MODE(Client *sender, std::stringstream &sstream)
{
	std::string channel_name;
	std::string mode;
	std::string str;
	
	getline(sstream, str, ' ');
	while(str.empty() == true && sstream.eof() == true)			//loop until #
		getline(sstream, str, ' ');
	if(str[0] == '#')
	{
		channel_name = str;
		//loop to + or - "mode"
		if(+)
			plus == true;
		mode = str[0];
		_channels[channel_name]->setMode(sender, mode);			//chatch error codes
	}
} */
