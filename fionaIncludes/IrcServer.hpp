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
#include <sstream>
#include <cstdlib>
#include <string>
#include <map>

class IrcServer
{
private:
	struct epoll_event	_ev;
	int		_epoll_fd;
	int		_sock_fd;

void	accept_connection();
void	process_event(const	int& client_sock);
void	failure_exit(const std::string& error_msg); //exits?


public:
//con- and destructer
	IrcServer();
	IrcServer(const IrcServer& S);
	IrcServer operator=(const IrcServer& S);
	~IrcServer();

//methods
	void	createTcpSocket(const std::string& ip, const int& port); //exits?
	void	createEpoll(); //exits?
	void	epollLoop(); //exits?
};
