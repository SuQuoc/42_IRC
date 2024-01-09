#include "../Srcs/Irc.hpp"

//con- and destructer
Irc();
Irc(const Irc& I);
Irc operator=(const Irc& I);
~Irc();

//private methods 
void	command_switch(std::string	command);

//methods (commands)
void	JOIN(Client *sender, std::stringstream sstream)
{
	std::string	channel_name;
	std::getline(sstream, channel_name, ' ');

	
}
void	PART(Client *sender, std::stringstream sstream);
void	QUIT(Client *sender, std::stringstream sstream);
void	KICK(Client *sender, std::stringstream sstream);

void	PASS(Client *sender, std::stringstream sstream);
void	NICK(Client *sender, std::stringstream sstream);
void	USER(Client *sender, std::stringstream sstream);
void	PRIVMSG(Client *sender, std::stringstream sstream);

void	MODE(Client *sender, std::stringstream sstream);
void	TOPIC(Client *sender, std::stringstream sstream);
void	INVITE(Client *sender, std::stringstream sstream);