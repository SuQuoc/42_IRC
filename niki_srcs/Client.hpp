#pragma once

#include <string>
#include <vector>

#include "Channel.hpp"

class Channel;

class Client
{

	private:

		std::string _user_name;
		std::string _nick_name;
		const int 	_client_fd;
		std::vector<Channel *> _channels;

		Client();

	public:

		Client(const int &client_fd);
		Client(const int &client_fd, const std::string &user_name, const std::string &nick_name);
		Client(const Client &C);
		Client operator=(const Client &C);
		~Client();

		void joinChannel( Channel *new_channel );

		//set and get
		const std::string &getUserName() const;
		const std::string &getNickName() const;
		const int &getClientFd() const;
		const std::vector<Channel *> &getChannels() const;
		void setUserName(const std::string user_name);
		void setNickName(const std::string nick_name);
};
