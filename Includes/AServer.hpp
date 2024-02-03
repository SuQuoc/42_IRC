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

#define OPER_IP "10.14.3.10"
#define OPER_PW "operPW"

class AServer
{
protected:
	std::map<std::string, Channel*>	_channels;
	std::map<std::string, Client*>	_client_names;
	std::map<int, Client*>			_client_fds;
	struct epoll_event	_ev;
	const std::string	_name;
	const std::string	_password;
	int		_epoll_fd;
	int		_sock_fd;

	typedef std::map<std::string, Channel*>::iterator	channel_map_iter_t;
	typedef std::map<std::string, Client*>::iterator	client_name_map_iter_t;
	typedef std::map<int, Client*>::iterator			client_fd_map_iter_t;
	typedef std::map<std::string, Channel*>::const_iterator		channel_map_const_it;
	typedef std::map<std::string, Client*>::const_iterator		client_name_map_const_it;
	typedef std::map<int, Client*>::const_iterator				client_fd_map_const_it;

//Canonical Form
	AServer();
	AServer(const std::string& name, const std::string& password);

//methods
	void	accept_connection();
	void	disconnectClient(const int& client_fd);
	void	disconnectClient(Client *client, const std::string& msg);

	void	process_event(const int& client_sock);
	int		printErrorReturn(const std::string& error_msg);
	virtual void	command_switch(Client *sender, const std::string message) = 0;

	void	addNewChannelToMap(Client *sender, std::string channel_name);
	void	addClientToNameMap(std::string user_name, const int& client_fd);
	void	addNewClientToFdMap(const int& client_fd, const std::string& client_ip);

	void 	rmClientFromNameMap(const std::string& nick_name);
	void 	rmClientFromMaps(Client *client); 
	void 	rmClientFromMaps(int client_fd);
	void 	rmChannelFromMap(std::string channel_name);

//getters
	client_fd_map_const_it		getClientIter(int fd) const;
	client_name_map_const_it	getClientIter(const std::string& name) const;
	channel_map_const_it		getChannelIter(std::string name) const;

	Client*		getClient(int fd) const;
	Client*		getClient(const std::string& name) const;
	Channel*	getChannel(const std::string& name) const;


public:
//con- and destructer

	virtual ~AServer();

//methods
	int		createTcpSocket(const int& port);
	int		createEpoll();
	void	epollLoop(); 
};
