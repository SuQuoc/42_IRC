#pragma once

#include <stdio.h> //fileno not allowed, header not needed without it!

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#include <iostream>
#include <cstring> //only strerror()
#include <sstream>
#include <cstdlib>
#include <string>
#include <map>

class Server
{
private:
	//client_name-map!
	//client_fd-map!
	//channel-map!
	struct epoll_event	_ev;
	int		_epoll_fd;
	int		_sock_fd;

//methods
void	accept_connection();
void	process_event(const	int& client_sock);
void	failure_exit(const std::string& error_msg); //exits?
virtual void	command_switch(std::string command) = 0;


public:
//con- and destructer
	Server();
	Server(const Server& S);
	Server operator=(const Server& S);
	~Server();

//methods
	void	createTcpSocket(const std::string& ip, const int& port); //exits?
	void	createEpoll(); //exits?
	void	epollLoop(); //exits?
};
