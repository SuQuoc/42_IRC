#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>

#include <iostream>
#include <string>
#include <map>

class IrcServer
{
private:
	struct epoll_event	_ev;
	int		_epoll_fd;
	int		_sock_fd;

void	failure_exit(const std::string& error_msg); //exits?
void	accept_connection();


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