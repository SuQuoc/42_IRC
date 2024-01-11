#include "../Includes/Irc.hpp"

//con- and destructer
Irc::Irc(): AServer() {}
Irc::Irc(std::string password): AServer(password) {}
/* Irc::Irc(const Irc& I);
Irc::Irc operator=(const Irc& I); */
Irc::~Irc() {}

//private methods 
void Irc::command_switch(Client *sender, const std::string& message) //request better name? for us to discern
{
    sstream(string)
    std::string cmd  = getline();

    if (cmd == "PASS") PASS() // client can always can try PASS although not registered
	//else if (sender->isRegistered() == false) sendError(ERR_);
    else if (cmd == "NICK") NICK()
    else if (cmd == "USER") USER()
    else if (cmd =="PRIVMSG") PRIVMSG()
    else if (cmd =="JOIN") JOIN()
    else if (cmd =="PART") PART()
    else if (cmd =="QUIT") QUIT()
    else if (cmd =="KICK") KICK()
    else if (cmd =="INVITE") INVITE()
    else if (cmd =="MODE") MODE()
    else if (cmd =="TOPIC") TOPIC()
    else sendError(ERR_UNKNOWNCOMMAND)
}


//methods (commands)
/* void	JOIN(Client *sender, std::stringstream &sstream)
{
	std::string	channel_name;
	std::getline(sstream, channel_name, ' ');
	
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

void	PART(Client *sender, std::stringstream &sstream)
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
void	QUIT(Client *sender, std::stringstream &sstream)
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
void	KICK(Client *sender, std::stringstream &sstream);



//WIR NEHMEN AN DAS wir immer PASS NICK USER bekommen 
//dh ich kann das bissi ändern
void	PASS(Client *sender, std::stringstream &sstream)
{
    std::string password;
    std::getline(sstream, password); //until newline default

    if (password.empty())
        sendError(ERR_);
    else if (sender->isRegistered())
        sendError(ERR_); //already registered
    else if (password == _serverPW) //gehört noch in Abstract Server
        sender->setToAuthenticated(); //bissi blöder name
    else
        sendError(ERR_);
        // delete the user von server already?

	NICK(sender, sstream);
	USER(sender, sstream);
}

//should we even check for the order or trust Hexchat --> trust Hexchat
void	NICK(Client *sender, std::stringstream &sstream)
{
    std::string nickname;
    std::getline(sstream, nickname); //until newline default
	//what if nick has space is it being ignored are is space not allowed?
    
    if (nickanme.empty())
        sendError(ERR_);
    //else if (sender.isAuthenticated() == false) //didnt do PASS before NICK 
    //    sendError(ERR_); //or return 
    else if (!isNormed(nickname)) // are spaces allowed? getline currently loops until \n
        sendError(ERR_); //or return 

    it = _fd_map.find(nickname) //key may not be used cuz it creates an entry --> actually good for us no?
    if (it == map.end()) //no one has the nickname
        sender.setNickname();
}

void	USER(Client *sender, std::stringstream &sstream)
{
    std::vector<std::string> info(4);

	if (sender->isRegistered())
		sendError(ERR_); //already registered

	//for loop a lil weird but fine for now
    for (std::vector<std::string>::it = it.begin(); it != info.end(); it++)
    {
        std::getline(sstream, *it, ' ');
		if (it->empty()) 
			sendError(ERR_); //need more params
		if (!isNormed(*it))
			sendError(ERR_); //invalid due to characters, lengths, etc
    }
    sender->setUser(info[0], info[1], info[2], info[3]);
	sender->registrationAccepted(); //setter i guess ? should also sendMsg(RPL_WELCOME); --> in 
	//add Client to map //server job
}
void	PRIVMSG(Client *sender, std::stringstream &sstream);

void	MODE(Client *sender, std::stringstream &sstream);
void	TOPIC(Client *sender, std::stringstream &sstream);
void	INVITE(Client *sender, std::stringstream &sstream); */