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
		std::string	_password;
		std::string	_topic;
		std::string	_name;
		int	_max_clients;

		typedef	std::vector<Channel::Member_t>::iterator clients_itr;

		void	sendNonBlock(const int &fd, const std::string &msg);
		clients_itr getClient(const Client *client);
		Channel();

	public:

		Channel(Client *channel_operator, const std::string &channel_name);
		Channel(const Channel &C);
		~Channel();

		void	rmClient(const Client *executor, const Client *rm_client);
		void	sendMsg(const Client *sender, const std::string &msg);
		void	addClient(Client *new_client, const std::string &password);
		void	addClient(Client *new_client);
		void	rmClient(const Client *rm_client);
		bool	isInChannel(const Client *client);
		bool	isOperator(const Client *client);

		void	setPassword(const std::string &password);
		void	setMaxClients(const int &max_clients);
		void	setName(const std::string &name);

		const std::string	&getPassword() const;
		const std::string	&getName() const;
		int size() const;

};
