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
void	IrcServer::accept_connection()
{
	struct sockaddr_in	client_addr;
	struct epoll_event	ev_temp;
	socklen_t			client_addr_len = sizeof(client_addr);
	int		client_sock;

	while (1)
	{
		client_sock = accept(_sock_fd, reinterpret_cast<struct sockaddr*>(&client_addr), &client_addr_len);
		if (client_sock == -1)
		{
			if (errno != EAGAIN && errno != EWOULDBLOCK)
				std::cerr << "Error: accept failed" << std::endl;
			return ;
		}
		ev_temp.events = EPOLLIN | EPOLLET; //needs to be in loop? //niki says it could be changed in epoll_ctl()
		fcntl(client_sock, F_SETFL, O_NONBLOCK);
		if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_sock, &ev_temp) == -1)
		{
			std::cerr << "Error: epoll_ctl failed in accept_connection" << std::endl;
			return ;
		}
	}
}

void	IrcServer::process_event(const int& client_sock)
{
	char	buf[513]; //13?	
	int		bytes_recieved; //better name?

	//recv needs to be in loop???????
	bytes_recieved = recv(client_sock, buf, sizeof(buf), 0);
	switch (bytes_recieved)
	{
		case (-1):
			std::cerr << "Error: couldn't recieve data";
			break ;
		case (0):
			//disconnect_client();
			close(client_sock);
			break ;
		default:
			std::cout << buf << std::endl;
	}
}

void	IrcServer::failure_exit(const std::string& error_msg)
{
	std::cout << "Error: " << error_msg << std::endl;
	std::exit(errno); //errno?
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
	if (setsockopt(_sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) //not necassery
		failure_exit("couldn't set socket options");
	if (bind(_sock_fd, reinterpret_cast<sockaddr*>(&saddr), sizeof(struct sockaddr_in)) == -1)
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

	while (1)
	{
		ev_cnt = epoll_wait(_epoll_fd, events, 1000, -1); //1000? //-1?
		if (ev_cnt == -1)
			failure_exit("epoll_wait failed"); //exits?
		for (int i = 0; i < ev_cnt; i++)
		{
			if (events[i].data.fd == _sock_fd)
				accept_connection();
			else if (events[i].data.fd == STDIN_FILENO)
				return ;
			//??????????v
			else if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN))) //???????????????
				close(events[i].data.fd);
			else
				process_event(events[i].data.fd); //recv
		}
	}
	//close client fds in epoll?
}