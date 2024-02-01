#pragma once

#include <sstream>
#include "AServer.hpp"
#include "IrcReply.hpp"

#define LIST_LIMIT 10


class AServer;

class Irc : public AServer
{
private:
	IrcReply _replier;
	
	std::string 	_op_host;
	std::string 	_op_password;

//methods
	Irc();

	void	command_switch(Client *sender, const std::string message);
	std::string	createMsg(Client *sender, const std::string& cmd, const std::string& recipient, const std::string& msg) const;
	bool	isChannelNameValid(const std::string &channel_name);

public:
//con- and destructer
	Irc(const std::string& name, const std::string& password);
	~Irc();

//methods (commands)
	int		JOIN(Client *sender, std::stringstream& sstream);
	int		PART(Client *sender, std::stringstream& sstream);
	void	QUIT(Client *sender, std::stringstream& sstream);
	int		KICK(Client *sender, std::stringstream& sstream);

	void	PASS(Client *sender, std::stringstream& sstream);
	void	NICK(Client *sender, std::stringstream& sstream);
	void	USER(Client *sender, std::stringstream& sstream);
	void	PRIVMSG(Client *sender, std::stringstream& sstream);

	void	TOPIC(Client *sender, std::stringstream& sstream);
	int		INVITE(Client *sender, std::stringstream& sstream);

	void	OPER(Client *sender, std::stringstream &sstream);
	int		KILL(Client *sender, std::stringstream &sstream);

	void setOperatorHost(const std::string& hostname);
	void setOperatorPW(const std::string& password);

//MODE
	int		MODE(Client *sender, std::stringstream& sstream);
	void	modesAreSetTo(Client *sender, Channel *channel);
	void	modesSwitch(Channel *channel, Client *sender, std::map<std::string, int> &operator_rpl_map, std::stringstream &sstream, const char &pre_fix, std::vector<int> &error_codes, const char &word_char);
	void	operatorsSendSetModeToChannel(Channel *channel, Client *sender, const std::map<std::string, int> &operator_rpl_map);
	void	sendSetModeToChannel(Channel *channel, Client *sender, const int &inv_code, const int &topic_code);

};
