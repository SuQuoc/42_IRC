#pragma once

#include <string>

class Client
{

	private:

		std::string _user_name;
		std::string _nick_name;
		const int 	_client_fd;

		Client();

	public:

		Client(const int &client_fd);
		Client(const int &client_fd, const std::string &user_name, const std::string &nick_name);
		Client(const Client &C);
		Client operator=(const Client &C);
		~Client();

		//set and get
		const std::string &getUserName();
		const std::string &getNickName();
		const int &getClientFd();
		void setUserName(const std::string user_name);
		void setNickName(const std::string nick_name);
};
