#pragma once

#include <sys/socket.h>     // ford send
#include <iostream>
#include <map>
#include <cstdlib>

#include "Client.hpp"

#define EXIT_FAILURE 1

class Client;

class Channel
{
	private:

		std::map<std::string, Client *> _operators;
		std::map<std::string, Client *> _members;		// should it be just a std::string or a ref to the client?? should there be a operator map/vector
		std::string _password;
		std::string _name;                        		// can we change it ?
		Client *_owner;									// should it be just a std::string or a ref to the client??

		
		Channel();

	public:
		typedef std::map<std::string, Client *>::iterator members_itr;

		Channel(Client *owner, const std::string &channel_name);
		Channel(const Channel &C);
		~Channel();

		void sendMsgToChannel(Client &sender, const std::string &msg);
		void kickFromChannel(Client &executor, Client &kick_client);
		void addClient(Client *new_member);
		void addClient(Client *new_client, bool is_operator);		// maybe we can do this in one funktion but it is one if less in non operatotr case

		void setName(const std::string &name);
		void setPassword(const std::string &password);
		const std::string &getPassword() const;
		const std::string &getName() const;
		const std::map<std::string, Client *> &getMembers()
		{
			return _members;
		}
};
