#pragma once

#include <sstream>
#include "AServer.hpp"
#include "IrcReply.hpp"

#define LIST_LIMIT 10


class AServer;

class Irc : public AServer
{
private:
	IrcReply	_replier;
	Client		*_sender;
	
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
	int		JOIN(std::stringstream& sstream);
	int		PART(std::stringstream& sstream);
	void	QUIT(std::stringstream& sstream);
	int		KICK(std::stringstream& sstream);
	int		WHO(std::stringstream& sstream);

	void	PASS(std::stringstream& sstream);
	int		NICK(std::stringstream& sstream);
	void	USER(std::stringstream& sstream);
	void	PRIVMSG(std::stringstream& sstream);

	void	TOPIC(std::stringstream& sstream);
	int		INVITE(std::stringstream& sstream);

	void	OPER(std::stringstream &sstream);
	int		KILL(std::stringstream &sstream);

	void	setOperatorHost(const std::string& hostname);
	void	setOperatorPW(const std::string& password);

//MODE
	int		MODE(std::stringstream& sstream);
	int		sendModesStatus(Channel *channel);
	int		modesSwitch(Channel *channel, std::map<std::string, int> &operator_rpl_map, std::stringstream &sstream, const char &pre_fix, std::vector<int> &error_codes, const char &word_char);
	int		sendKeyRPLorError(Channel *channel, std::string &argument, const int &key_code, const char &pre_fix);
	void	operatorsSendSetModeToChannel(Channel *channel, Client *sender, const std::map<std::string, int> &operator_rpl_map);
	void	sendSetModeToChannel(Channel *channel, Client *sender, const int &inv_code, const int &topic_code);

	void	setSender(Client *sender);
};
