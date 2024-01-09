#include "../Srcs/Irc.hpp"

//con- and destructer
Irc();
Irc(const Irc& I);
Irc operator=(const Irc& I);
~Irc();

//private methods 
void	command_switch(std::string	command);

//methods (commands)
void	JOIN(Client *sender, std::stringstream &sstream)
{
	std::string	channel_name;
	std::getline(sstream, channel_name, ' ');
	
}
void	PART(Client *sender, std::stringstream &sstream);
void	QUIT(Client *sender, std::stringstream &sstream)
{
	std::string	channel_name;
	std::string	msg;
	std::getline(sstream, msg); //make message! //or do it in channel-object?

	//get const channel-vector reference from client-map(?)
	//loop through channel-vector
		//if (channel-vector[i]->removeClient(sender, msg) == CHANNEL_DEAD);
			//get channel_name from channel-vector[i]
			//delete channel through channel-map (channel_name);
			//erase channel from channel-map
		//sender->leaveChannel(channel-vector[i]);
	//erase client from fd_client-map
	//get client_name (-> channel_name variable?)
	//delete client-object (sender)
	//erase client from name_client-map (name)
}
void	KICK(Client *sender, std::stringstream &sstream);

void	PASS(Client *sender, std::stringstream &sstream);
void	NICK(Client *sender, std::stringstream &sstream);
void	USER(Client *sender, std::stringstream &sstream);
void	PRIVMSG(Client *sender, std::stringstream &sstream);

void	MODE(Client *sender, std::stringstream &sstream);
void	TOPIC(Client *sender, std::stringstream &sstream);
void	INVITE(Client *sender, std::stringstream &sstream);