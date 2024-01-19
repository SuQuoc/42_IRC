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
	else if (cmd == "JOIN") std::cout << "JOIN()" << std::endl; //JOIN();
	else if (cmd == "PART") std::cout << "PART()" << std::endl; //PART();
	else if (cmd == "QUIT") std::cout << "QUIT()" << std::endl; //QUIT();
	else if (cmd == "KICK") std::cout << "KICK()" << std::endl; //KICK();
	else if (cmd == "INVITE") std::cout << "INVITE()" << std::endl; //INVITE();
	else if (cmd == "MODE") std::cout << "MODE()" << std::endl; //MODE();
	else if (cmd == "TOPIC") std::cout << "TOPIC()" << std::endl; //TOPIC();
	else std::cout << "sendError(ERR_UNKNOWNCOMMAND)" << std::endl; //sendError(ERR_UNKNOWNCOMMAND);
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

	if (word.empty() == false && (*(word.end() - 1) == '\n' || *(word.end() - 1) == '\r'))
	{
		word.erase(word.end() - 1);
	}
	if (!word.empty() && (*(word.end() - 1) == '\n' || *(word.end() - 1) == '\r'))
		word.erase(word.end() - 1);
	return (word);
}

//methods (commands)
// void	Irc::JOIN(Client *sender, std::stringstream &sstream)
// {
// 	std::string	channel_name = getWord(sstream);
	
// 	check if channel exists
// 		if not, check if channel_name is valid
// 		if yes, make channel //addNewPair(sender, channel_name);
// 		if not, error
// 	check if client is in channel
// 		if yes, error
// 	if not, try adding client to channel
// 		error ?
// 	add channel to channel vector in user
// }
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


//WIR NEHMEN AN DAS wir immer PASS NICK USER bekommen 
//dh ich kann das bissi ändern
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
	}
	return ;
}

//should we even check for the order or trust Hexchat --> trust Hexchat
//OLD; else if (sender.isAuthenticated() == false) //didnt do PASS before NICK 
    //    sendError(ERR_NOSUCHCHANNEL, sender); //or return 
    // else if (!isNormed(nickname)) //
        // sendError(ERR_NOSUCHCHANNEL, sender); //or return 
void Irc::NICK(Client *sender, std::stringstream &sstream)
{
    std::string nickname = extractWord(sstream);
    //if (sender->isAuthenticated() == false) //didnt do PASS before NICK 
    //    sendError(321, sender) and return; //theres no err_code for it
	//protocol says u must send PASS before NICK and USER but what if client doesnt, what should be send back


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
		sendError(ERR_NICKNAMEINUSE, sender, "");
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
    sender->setUser(info[0], info[1], info[2], info[3]);
	if (sender->isRegistered())
	{
		addClientToNameMap(sender->getNickname(), sender->getFd());
		sendRPL(RPL_WELCOME, sender, sender->getUsername());
	}
}



// ERR_NORECIPIENT
// ERR_NOTEXTTOSEND 412
// ERR_CANNOTSENDTOCHAN 404 WEIRD READ IT
// ERR_TOOMANYTARGETS ?? I DONT WANT TO COVER THAT
// ERR_NOSUCHNICK
void Irc::PRIVMSG(Client *sender, std::stringstream &sstream)
{
	std::string recipient;
	std::string message;
    std::getline(sstream, recipient, ' '); //space 
    std::getline(sstream, message); //semicolon ignored at the moment

	message = "PRIVMSG " + recipient + " " + message;
	
	// SHOULD WE CHECK FOR:
		// - ':' ?? -> hexchat doesnt let u write /PRIVMSG
		// - if recpient == sender.getNickname()? -> u can write yourself a message in hexchat with /PRIVMSG
		// - /PRIVMSG for channels didnt work in Hexchat

	if (recipient.empty()) 
		sendError(ERR_NORECIPIENT, sender, ""); //need more params
	else if (message.empty())
		sendError(ERR_NOTEXTTOSEND, sender, ""); //need more params
	else if (recipient[0] == '#')
	{
		channel_map_iter_t rec_it = _channels.find(recipient);
		if (rec_it == _channels.end())
			sendError(ERR_NOSUCHCHANNEL, sender, ""); //NO SUCH CHANNEL
		else
		{
			sender->sendTo(message, rec_it->second); //Client should call a function from Channel
			//it->second->NiksFunctionToRelay(sender, message);
			//if message is being concatinated in channel it maybe has to know what IRC CMD was triggered
			//then u can use the NiksFunctionToRelay(sender, message) only for one specific IRC CMD and not for e.g. both PRIVMSG and JOIN
		}
	}
	else
	{
		client_name_map_iter_t rec_it = _client_names.find(recipient);
		if (rec_it == _client_names.end())
			sendError(ERR_NOSUCHNICK, sender, ""); //NO SUCH NICK
		else
			sender->sendTo(message, rec_it->second); //this function calls Channel member
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
