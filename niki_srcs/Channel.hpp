#pragma once

#include <sys/socket.h>     // ford send
#include <iostream>
#include <map>

#include "Client.hpp"

#define EXIT_FAILURE 1

class Channel
{
	private:

		std::map<std::string, Client &> members;        // should it be just a std::string or a ref to the client?? should there be a operator map/vector
		const std::string _name;                        // can we change it ?
		Client &_owner;									// should it be just a std::string or a ref to the client??

		typedef std::map<std::string, Client &>::iterator members_itr;

	public:

		Channel(Client &owner, const std::string &channel_name);

		void sendMsgToChannel(Client &sender, const std::string &msg);
		void addClient(Client &new_member);
};