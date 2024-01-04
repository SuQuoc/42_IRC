#include "../fionaIncludes/IrcServer.hpp"

//con- and destructer
IrcServer::IrcServer(): _epoll_fd(-1), _sock_fd(-1) {}
IrcServer::IrcServer(const IrcServer& S)
{
	*this = S;
}
IrcServer IrcServer::operator=(const IrcServer& S)
{
	_ev.data.fd = S._ev.data.fd;
	_ev.data.ptr = S._ev.data.ptr;
	_ev.data.u32 = S._ev.data.u32;
	_ev.data.u64 = S._ev.data.u64;
	_ev.events = S._ev.events;

	_epoll_fd = S._epoll_fd;
	_sock_fd = S._sock_fd;
	return (*this);
}
IrcServer::~IrcServer()
{
	if (_epoll_fd != -1)
		close(_epoll_fd);
	if (_sock_fd != -1)
		close(_sock_fd);
}

//private methods
void	IrcServer::failure_exit(const std::string& error_msg)
{
	std::cout << "Error: " << error_msg << std::endl;
	std::exit(errno); //errno?
}

void	IrcServer::accept_connection()
{

}

//public methods
void	IrcServer::createTcpSocket(const std::string& ip, const int& port) //exits?
{
	struct sockaddr_in saddr;
	int	optval = 1;

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(8080); //needs to be changeble //8080?
	saddr.sin_addr.s_addr = INADDR_ANY; //needs to be changeble //INADDR_ANY?

	_sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //IPPROTO_TCP?
	if (_sock_fd == -1)
		failure_exit("couldn't create socket");
	if (setsockopt(_sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
		failure_exit("couldn't set socket options");
	if (bind(_sock_fd, (struct sockaddr*)&saddr, sizeof(struct sockaddr_in)) == -1)
		failure_exit("couldn't bind to socket");
	if (listen(_sock_fd, 1000) == -1) //1000?
		failure_exit("couldn't bind to socket");
	if (fcntl(_sock_fd, F_SETFL, O_NONBLOCK) == -1)
		failure_exit("fcntl failed");
}

void	IrcServer::createEpoll()
{
	_ev.data.fd = _sock_fd;
	_ev.events = EPOLLIN | EPOLLET; //ECOLLET sets Edge-Triggered mode

	_epoll_fd = epoll_create1(0); //can be set to EPOLL_NONBLOCK
	if (_epoll_fd == -1)
		failure_exit("couldn't create epoll");
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _sock_fd, &_ev) == -1)
		failure_exit("epoll_ctrl failed");
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &_ev) == -1) //to get out of loop(?)
		failure_exit("epoll_ctrl failed on stdin");
}

void	IrcServer::epollLoop()
{
	struct epoll_event	events[1000]; //1000?
	int 	ev_cnt;

	while ()
	{
		ev_cnt = epoll_wait(_epoll_fd, events, 1000, -1); //1000? //-1?
		if (ev_cnt == -1)
			failure_exit("epoll_wait failed"); //exits?
		for (int i = 0; i < ev_cnt; i++)
		{
			if (events[i].data.fd == _sock_fd)
				//accept_connection();
			if (events[i].data.fd == STDIN_FILENO)
				return ;
			//??????????v
			else if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN))) //???????????????
				close(events[i].data.fd);
			else
				process_event(events[i].data.fd); //recv
		}
	}
}