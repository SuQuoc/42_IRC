#pragma once

#include <sys/socket.h>     // ford send
#include <iostream>
#include <cstdlib>			// exit
#include <vector>
#include <cerrno>

#include "../Includes/Client.hpp"

#define EXIT_FAILURE 1
#define MAX_CLIENTS 100 // How high should it be?

class Client;

class Channel
{
	private:

		struct Member_t 
		{
			Client *members; 
			bool is_operator;
		};

		std::vector<Member_t> _clients;
		std::string	_password;			//if pw is empty channel is in non password mode
		std::string	_topic;
		std::string	_name;

		bool _restrict_topic;
		bool _invite_only;
		
		int	_max_clients;

		typedef	std::vector<Channel::Member_t>::iterator clients_itr;

		void	sendNonBlock(const int &fd, const std::string &msg);
		clients_itr getClient(const Client *client);
		clients_itr Channel::getClient(const std::string _name);
		Channel();

	public:

		Channel(Client *channel_operator, const std::string &channel_name);
		Channel(const Channel &C);
		~Channel();


		void	rmClient(const Client *executor, const Client *rm_client);
		void	sendMsg(const Client *sender, const std::string &msg);
		void	addClient(Client *new_client, bool is_operator);
		void	rmClient(const Client *rm_client);
		bool	isInChannel(const Client *client);
		bool	isOperator(const Client *client);

		void	setPassword(const std::string &password);
		void	setMaxClients(const int &max_clients);
		void	setName(const std::string &name);

		const std::string	&getPassword() const;
		const std::string	&getName() const;
		int size() const;

		void Channel::changeChannelMode(Client *executor, const std::string &mode, const std::string &argument);
};
