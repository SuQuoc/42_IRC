#pragma once

#include <stdio.h> //fileno not allowed, header not needed without it!

#include "Channel.hpp"
#include "Client.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>

#include <iostream>
#include <iterator>
#include <cstring> //only strerror()
#include <sstream>
#include <cstdlib>
#include <string>
#include <map>

class AServer
{
protected: //protected? -> yes, if not i would need a getter for everything
	std::map<std::string, Channel*>	_channels;
	std::map<std::string, Client*>	_client_names;
	std::map<int, Client*>			_client_fds;
	struct epoll_event	_ev;
	const std::string	_password;
	int		_epoll_fd;
	int		_sock_fd;
	//server name? --> passed in constructor
	typedef std::map<std::string, Channel*>::iterator	channel_map_iter_t;
	typedef std::map<std::string, Client*>::iterator	client_name_map_iter_t;
	typedef std::map<int, Client*>::iterator			client_fd_map_iter_t;

//Canonical Form
	AServer(); //? 
	AServer(std::string password);
//methods
	void	accept_connection();
	void	process_event(const int& client_sock);
	void	failure_exit(const std::string& error_msg); //exits?
	struct addrinfo*	getIpAdressToBind(const int& port);
	virtual void	command_switch(Client *sender, const std::string message, const int& new_client_fd) = 0;

	void	addNewPair(Client *sender, const std::string& channel_name);
	// void	addNewPair(std::string user_name, const int& client_fd); //add more?
	void	addClientToNameMap(std::string user_name, const int& client_fd); //add more?
	void	addClientToFdMap(const int& client_fd);
	void 	rmClient(Client *client); 
	void 	rmClient(int client_fd); //which one makes more sense, do we need it for name too?
	void 	rmChannel(const std::string& channel_name);

public:
//con- and destructer
	
	//AServer(const AServer& S); ?
	//AServer operator=(const AServer& S); //not possible?
	virtual ~AServer(); //put this under protected?

//methods
	void	createTcpSocket(const int& port); //exits?
	void	createEpoll(); //exits?
	void	epollLoop(); //exits?
};
