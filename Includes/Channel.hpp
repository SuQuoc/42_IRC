#pragma once

#include <sys/socket.h>     // ford send
#include <iostream>
#include <cstdlib>			// exit
#include <vector>

#include "../Includes/Client.hpp"

#define EXIT_FAILURE 1

class Client;

class Channel
{
	private:

		std::vector<Client *> _operators;
		std::vector<Client *> _members;
		std::string _password;
		std::string _topic;
		std::string _name;
		int _client_limit;

		Channel();

		typedef std::vector<Client *>::iterator clients_itr;

	public:

		Channel(Client *channel_operator, const std::string &channel_name);
		Channel(const Channel &C);
		~Channel();

		void sendMsgToChannel(const Client *sender, const std::string &msg);
		void rmClient(const Client *executor, Client *rm_client);
		void rmClient(Client *rm_client);

		void addMember(Client *new_member);
		void addOperator(Client *new_operator);		// maybe we can do this in one funktion but it is one if less in non operatotr case
		bool isOperator(const Client *client);

		void setName(const std::string &name);
		void setPassword(const std::string &password);

		const std::string &getPassword() const;
		const std::string &getName() const;
		std::vector<Client *>::iterator Channel::getMember(const std::string &name);
		std::vector<Client *>::iterator Channel::getOperator(const std::string &name);
};
