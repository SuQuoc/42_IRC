#include "../Includes/Irc.hpp"

//con- and destructer
Irc::Irc(): AServer() {}
Irc::Irc(std::string password): AServer(password) {}
/* Irc::Irc(const Irc& I);
Irc::Irc operator=(const Irc& I); */
Irc::~Irc() {}

enum IRC_ERR
{
    ERR_NOSUCHCHANNEL = 403, //? can this happen with our Reference client, when does it occur
    ERR_CANNOTSENDTOCHAN = 404, //?
    ERR_UNKNOWNCOMMAND = 421,
    ERR_ERRONEUSNICKNAME = 432,
    ERR_NICKNAMEINUSE = 433,
    ERR_USERNOTINCHANNEL = 441, //?
    ERR_NOTONCHANNEL = 442, //?
    ERR_USERONCHANNEL = 443,
    ERR_NOTREGISTERED = 451,
    ERR_NEEDMOREPARAMS = 461,
    ERR_ALREADYREGISTERED = 462,
    ERR_PASSWDMISMATCH = 464,
    ERR_UNKNOWNMODE = 472, //?
    ERR_CHANOPRIVSNEEDED = 482, 
    ERR_USERMODEUNKNOWNFLAG = 501, //?
    ERR_USERSDONTMATCH = 502 //?
};

//private methods 
void	Irc::command_switch(Client *sender, const std::string message) //message-> 'request' better name? for us to discern
{
    std::stringstream	sstream(message); //message can't be empty
    std::string	cmd;

	std::getline(sstream, cmd, ' ');

	std::cout << "cmd: " << cmd << std::endl;
	if (sender == NULL) //doesn't protect when sender is not in map?
	{
		std::getline(sstream, cmd); //? for CAP LS 
		std::getline(sstream, cmd, ' '); //?
		//std::cout << "cmd2: " << cmd << std::endl;
		if (cmd == "PASS")
			std::cout << "PASS()" << std::endl; //PASS(); // client can always try PASS although not registered ?
		//else if (sender->isRegistered() == false) sendError(1, sender); ?
		else
			std::cerr << "* Error: sender is NULL (in command_switch)" << std::endl;
		return ;
	}
	//else if (sender->isRegistered() == false) sendError(1, sender); ?
	else if (cmd == "NICK") std::cout << "NICK()" << std::endl; //NICK();
	else if (cmd == "USER") std::cout << "USER()" << std::endl; //USER();
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


//methods (commands)
void Irc::JOIN(Client *sender, std::stringstream &sstream)
{
	std::string	channel_name;
	std::getline(sstream, channel_name, ' '); //rest is being ignored bc u cant change the join message
	
	
	
	_channels.find(channel_name);
	
}
std::string	make_message(sender, msg, default)
{
	std::string	message;

	give message sender info
	if (msg.empty())
		message + default
	message + msg;
	return (message);
}

void Irc::PART(Client *sender, std::stringstream &sstream)
{
	//PART #channnel-name msg
	Channel		*channel;
	std::string	channel_name;
	std::string	msg;

	std::getline(sstream, channel_name);
	std::getline(sstream, msg); //make message! //or do it in channel-object?
	channel = sender.getChannel(channel_name)
	if (channel == std::nullptr)
		//send error to sender //or return if send is in client-class
	if (channel_map.find(channel_name)->removeClient(sender, msg) == CHANNEL_DEAD); //delete channel when empty()
			//get channel_name from channel-vector[i] //not needed here
			delete channel through channel-map (channel_name);
			erase channel from channel-map
	sender->leaveChannel();
	channel.sendMsg(sender, make_message(sender, msg, default));
}
void Irc::QUIT(Client *sender, std::stringstream &sstream)
{
	std::string	channel_name;
	std::string	msg;
	std::getline(sstream, msg); //make message! //or do it in channel-object?

	get const channel-vector reference from client-map(?)
	loop through channel-vector
		if (channel-vector[i]->removeClient(sender, msg) == CHANNEL_DEAD); //delete channel when empty()
			get channel_name from channel-vector[i]
			delete channel through channel-map (channel_name);
			erase channel from channel-map
		sender->leaveChannel(channel-vector[i]);
	erase client from fd_client-map
	get client_name (-> channel_name variable?)
	delete client-object (sender)
	erase client from name_client-map (name)
}
void Irc::KICK(Client *sender, std::stringstream &sstream);
{

}


//WIR NEHMEN AN DAS wir immer PASS NICK USER bekommen 
//dh ich kann das bissi ändern
void Irc::PASS(Client *sender, std::stringstream &sstream, int fd)
{
    std::string password;
    std::getline(sstream, password); //until newline defaul, according to chatGPT the server takes in the entire string with spaces

    //if (password.empty()) //wont be triggered through HexChat it would only send NICK and USER
	//	std::cout << "Empty PW" << std::endl;
	//    sendError(1, sender);
    if (sender != NULL && sender->isRegistered()) 
        sendError(1, sender); //already registered
    else if (password == _password) //gehört noch in Abstract Server
    {
		_client_fds[fd] = new Client(fd); //should we delete him if NICK or USER triggers an Error?
		//OLD; sender->setToAuthenticated(); 
	}
}

//should we even check for the order or trust Hexchat --> trust Hexchat
void Irc::NICK(Client *sender, std::stringstream &sstream)
{
    std::string nickname;
    std::getline(sstream, nickname);
	//what if nick has space is it being ignored are is space not allowed? --> NO
    
    if (nickname.empty())
        sendError(1, sender);
    //OLD; else if (sender.isAuthenticated() == false) //didnt do PASS before NICK 
    //    sendError(1, sender); //or return 
    //else if (!isNormed(nickname)) // are spaces allowed? getline currently loops until \n
    //    sendError(1, sender); //or return 

	client_fd_map_iter_t it = _client_fds.find(nickname); //key may not be used cuz it creates an entry --> actually good for us no?
    if (it == _client_fds.end()) //no one has the nickname
        sender->setNickname(nickname); //should setter norm check and return?
	if (sender->isRegistered())
		this->addNewPair(sender->getNickname(), sender->getFd());
}

void Irc::USER(Client *sender, std::stringstream &sstream)
{
    std::vector<std::string> info(4);

	if (sender->isRegistered())
		sendError(1, sender); //already registered

	//for loop a lil weird but fine for now
    for (std::vector<std::string>::iterator it = info.begin(); it != info.end(); it++)
    {
        std::getline(sstream, *it, ' ');
		if (it->empty()) 
			sendError(1, sender); //need more params
		if (!isNormed(*it))
			sendError(1, sender); //invalid due to characters, lengths, etc
    }
    sender->setUser(info[0], info[1], info[2], info[3]);
	//sender->registrationAccepted(); //setter i guess ? should also sendMsg(RPL_WELCOME); --> in 
	if (sender->isRegistered())
	{
		this->addNewPair(sender->getNickname(), sender->getFd());
		std::cout << "SEND Welcome to " << sender->getPrefix() << std::endl;
	}
	//add Client to map //server job
}
void Irc::PRIVMSG(Client *sender, std::stringstream &sstream);
{
	std::string recipient;
	std::string message;
    std::getline(sstream, recipient, ' '); //space 
    std::getline(sstream, message); //semicolon ignored at the moment


	// SHOULD WE CHECK FOR:
		// - ':' ?? -> hexchat doesnt let u write /PRIVMSG
		// - if recpient == sender.getNickname()? -> u can write yourself a message in hexchat with /PRIVMSG
		// - /PRIVMSG for channels didnt work in Hexchat

	if (recipient->empty()) 
		sendError(1, sender); //need more params
	else if (recipient[0] == '#')
		channel_map_iter_t rec_it = _channels.find(recipient);
		if (it == _channels.end())
			sendError(1, sender); //NO SUCH CHANNEL
		else
			sender->sendTo() //Client should call a function from Channel
			it->second->NiksFunctionToRelay(sender, message);
		//if message is being concatinated in channel it maybe has to know what IRC CMD was triggered
		//then u can use the NiksFunctionToRelay(sender, message) only for one specific IRC CMD and not for e.g. both PRIVMSG and JOIN
	else
	{
		client_name_map_iter_t rec_it = _client_names.find(recipient);
		if (it == _client_names.end())
			sendError(1, sender); //NO SUCH NICK
		else
			sender->sendTo(message, Client)
			_client_names.find(recipient)->second->ClientMemberFunction(message)?
	}
}


void Irc::MODE(Client *sender, std::stringstream &sstream);
void Irc::TOPIC(Client *sender, std::stringstream &sstream);
void Irc::INVITE(Client *sender, std::stringstream &sstream);