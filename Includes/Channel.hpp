#pragma once

#include <sys/socket.h>     // ford send
#include <iostream>
#include <cstdlib>			// exit
#include <vector>
#include <cerrno>
#include <algorithm>		// std::find

#include "../Includes/Client.hpp"
#include "IrcReply.hpp"

#define MAX_CLIENTS 100 // How high should it be?
#define EXIT_FAILURE 1
#define CH_SUCCESS 0
#define DELETE_CHANNEL -1

class Client;

class Channel
{
	private:

		struct Member_t 
		{
			Client *members; 
			bool is_operator;
		};

		std::vector<Member_t>	_clients;
		std::vector<Client*>	_invited;
		std::string	_password;			//if pw is empty channel is in non password mode
		std::string	_topic;
		std::string	_name;

		bool _restrict_topic;
		bool _invite_only;

		int	_max_clients;

		typedef	std::vector<Channel::Member_t>::iterator clients_itr;
		typedef	std::vector<Client*>::iterator invited_itr;

		int		changeMode(const char &add, bool &modes);
		clients_itr	getClient(const std::string &name);
		clients_itr	getClient(const Client *client);
		invited_itr	getInvited(const Client *client);
		Channel();

	public:

		Channel(Client *channel_operator, const std::string &channel_name);
		Channel(const Channel &C);
		~Channel();

		int		addInvited(Client *client);
		bool	isInvited(const Client*client);
		int		rmClient(const Client *executor, const Client *rm_client, const std::string &leaving_msg);
		int		addClient(Client *new_client, const std::string &password, bool is_operator);
		int		rmClient(const Client *rm_client, const std::string &leaving_msg);
		void	sendMsg(const Client *sender, const std::string &msg);
		bool	isInChannel(const Client *client);
		bool	isOperator(const Client *client);

		int		setTopic(const std::string &name, const std::string &topic);
		void	setName(const std::string &name);
		void	setMaxClients(const int &max_clients);
		int		setMaxClients(const std::string &str, const char &pre_fix);
		int		setPassword(const std::string &password, const char &add);

		const std::string	&getPassword() const;
		const std::string	&getTopic() const;
		const std::string	&getName() const;
		bool	getRestrictTopic() const;
		bool	getInviteOnly() const;
		int		getMaxClients() const;
		int		size() const;

		int modesSwitch(const char &add, const char &mode, const std::string &argument);

		int setOperator(const char &add, Client *client);
};
