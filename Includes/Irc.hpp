#pragma once

#include "AServer.hpp"

#include <sstream>
class AServer;

class Irc : public AServer
{
private:
//methods
	void	command_switch(/* std::string	command */);


public:
//con- and destructer
	Irc();
	Irc(std::string password);
 	//Irc(const Irc& I); ?
	//Irc operator=(const Irc& I); ?
	~Irc();

//methods (commands)
/* 	void	JOIN(Client *sender, std::stringstream& sstream);
	void	PART(Client *sender, std::stringstream& sstream);
	void	QUIT(Client *sender, std::stringstream& sstream);
	void	KICK(Client *sender, std::stringstream& sstream);

	void	PASS(Client *sender, std::stringstream& sstream);
	void	NICK(Client *sender, std::stringstream& sstream);
	void	USER(Client *sender, std::stringstream& sstream);
	void	PRIVMSG(Client *sender, std::stringstream& sstream);

	void	MODE(Client *sender, std::stringstream& sstream);
	void	TOPIC(Client *sender, std::stringstream& sstream);
	void	INVITE(Client *sender, std::stringstream& sstream); */
};
