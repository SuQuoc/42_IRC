#pragma once

#include <sstream>
#include "AServer.hpp"
#include "Irc_error.hpp"

class AServer;

class Irc : public AServer
{
private:
	std::string 	_op_host;
	std::string 	_op_password;
//methods
	void	command_switch(Client *sender, const std::string message, const int& new_client_fd);
	std::string createMsg(Client *sender, const std::string& cmd, const std::string& recipient, const std::string& msg) const;
public:
//con- and destructer
	Irc(); //should be priv!!
	Irc(std::string password);
 	//Irc(const Irc& I); ?
	//Irc operator=(const Irc& I); ?
	~Irc();

//methods (commands)
	int		JOIN(Client *sender, std::stringstream& sstream);
	void	PART(Client *sender, std::stringstream& sstream);
	void	QUIT(Client *sender, std::stringstream& sstream);
	// void	KICK(Client *sender, std::stringstream& sstream);

	void	PASS(Client *sender, std::stringstream& sstream, const int& new_client_fd);
	void	NICK(Client *sender, std::stringstream& sstream);
	void	USER(Client *sender, std::stringstream& sstream);
	void	PRIVMSG(Client *sender, std::stringstream& sstream);

	// void	MODE(Client *sender, std::stringstream& sstream);
	void	TOPIC(Client *sender, std::stringstream& sstream);
	// void	INVITE(Client *sender, std::stringstream& sstream);
	void	OPER(Client *sender, std::stringstream &sstream);
//methods (commands)
	void setOperatorHost(const std::string& hostname);
	void setOperatorPW(const std::string& password);
};
