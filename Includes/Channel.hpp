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

		typedef struct Member_t
		{
			Client *members;
			bool is_operator;
		};

		std::vector<Member_t> _clients;
		std::string	_password;
		std::string	_topic;
		std::string	_name;
		int	_max_clients;

		Channel();

		void	sendNonBlock(const int &fd, const std::string &msg);
		void	addClient(Client *new_client, bool is_operator);
		
		typedef	std::vector<Channel::Member_t>::iterator clients_itr;

	public:

		Channel(Client *channel_operator, const std::string &channel_name);
		Channel(const Channel &C);
		~Channel();

		void	sendMsg(const Client *sender, const std::string &msg);

		void	rmClient(const Client *executor, const Client *rm_client);
		void	rmClient(const Client *rm_client);
		void	addMember(Client *new_member);
		void	addOperator(Client *new_operator);		// maybe we can do this in one funktion but it is one if less in non operatotr case
		bool	isOperator(const Client *client);

		void	setPassword(const std::string &password);
		void	setMaxClients(const int &max_clients);
		void	setName(const std::string &name);

		std::vector<Member_t>::iterator Channel::getClient(const Client *client);
		const std::string	&getPassword() const;
		const std::string	&getName() const;
		int size() const;
};
