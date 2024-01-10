#include "../Includes/Irc.hpp"

//con- and destructer
Irc::Irc(): AServer() {}
Irc::Irc(std::string password): AServer(password) {}
/* Irc::Irc(const Irc& I);
Irc::Irc operator=(const Irc& I); */
Irc::~Irc() {}

//private methods 
void	Irc::command_switch(/* std::string	command */) {std::cout << "command switch" << std::endl;}

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

void	PASS(Client *sender, std::stringstream &sstream);
void	NICK(Client *sender, std::stringstream &sstream);
void	USER(Client *sender, std::stringstream &sstream);
void	PRIVMSG(Client *sender, std::stringstream &sstream);

void	MODE(Client *sender, std::stringstream &sstream);
void	TOPIC(Client *sender, std::stringstream &sstream);
void	INVITE(Client *sender, std::stringstream &sstream); */