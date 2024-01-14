#pragma once

#include "AServer.hpp"

#include <sstream>
#include "Irc_error.hpp"

class AServer;

class Irc : public AServer
{
private:
//methods
	void	command_switch(Client *sender, const std::string message, const int& new_client_fd);
	std::string	getWord(std::stringstream& sstream);


public:
//con- and destructer
	Irc();
	Irc(std::string password);
 	//Irc(const Irc& I); ?
	//Irc operator=(const Irc& I); ?
	~Irc();

//methods (commands)
	void	JOIN(Client *sender, std::stringstream& sstream);
	// void	PART(Client *sender, std::stringstream& sstream);
	// void	QUIT(Client *sender, std::stringstream& sstream);
	// void	KICK(Client *sender, std::stringstream& sstream);

	void	PASS(Client *sender, std::stringstream& sstream, const int& new_client_fd);
	void	NICK(Client *sender, std::stringstream& sstream);
	void	USER(Client *sender, std::stringstream& sstream);
	void	PRIVMSG(Client *sender, std::stringstream& sstream);

	// void	MODE(Client *sender, std::stringstream& sstream);
	// void	TOPIC(Client *sender, std::stringstream& sstream);
	// void	INVITE(Client *sender, std::stringstream& sstream);


	// Error switch
	void	sendError(IRC_ERR error, Client* sender) const;
};
