#include "../Includes/AServer.hpp"

//con- and destructer
AServer::AServer(): _password(""), _epoll_fd(-1), _sock_fd(-1) {}
AServer::AServer(std::string password): _password(password), _epoll_fd(-1), _sock_fd(-1) {}
/* AServer::AServer(const AServer& S)
{
	for (int i = 0; i < S._channels.size(); i++)
	{

	}
}
AServer AServer::operator=(const AServer& S)
{
	_ev.data.fd = S._ev.data.fd;
	_ev.data.ptr = S._ev.data.ptr;
	_ev.data.u32 = S._ev.data.u32;
	_ev.data.u64 = S._ev.data.u64;
	_ev.events = S._ev.events;

	_epoll_fd = S._epoll_fd;
	_sock_fd = S._sock_fd;
	return (*this);
} */
AServer::~AServer()
{
	for (channel_map_iter_t it = _channels.begin(); it != _channels.end(); it++)
		delete it->second;
	for (client_name_map_iter_t it = _client_names.begin(); it != _client_names.end(); it++)
		delete it->second;
	for (client_fd_map_iter_t it = _client_fds.begin(); it != _client_fds.end(); it++)
		delete it->second;
	if (_epoll_fd != -1)
		close(_epoll_fd);
	if (_sock_fd != -1)
		close(_sock_fd);
}

//private methods
void	AServer::accept_connection()
{
	struct sockaddr_in	client_addr;
	struct epoll_event	ev_temp;
	socklen_t			client_addr_len = sizeof(client_addr);
	int		client_fd;

	while (1)
	{
		client_fd = accept(_sock_fd, reinterpret_cast<struct sockaddr*>(&client_addr), &client_addr_len);
		if (client_fd == -1)
		{
			if (errno != EAGAIN && errno != EWOULDBLOCK)
				std::cerr << "Error: accept failed" << std::endl;
			return ;
		}
		ev_temp.data.fd = client_fd;
		ev_temp.events = EPOLLIN | EPOLLET; //needs to be in loop? //niki says it could be changed in epoll_ctl() so just to be save
		fcntl(client_fd, F_SETFL, O_NONBLOCK);
		if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &ev_temp) == -1)
		{
			std::cerr << "Error: epoll_ctl failed in accept_connection" << std::endl;
			return ;
		}
	}
}

//what happens if client presses ctrl-D? -> Subject!!!
void	AServer::process_event(const int& client_fd)
{
	char	buf[513]; //13?	
	int		bytes_recieved = -1; //better name?

	while (1)
	{
		bytes_recieved = recv(client_fd, buf, sizeof(buf), 0);
		switch (bytes_recieved)
		{
			case (-1):
				std::cerr << "Error: couldn't recieve data :" << std::strerror(errno) << std::endl;
				if (errno == EAGAIN || errno == EWOULDBLOCK) //leave it in? //potential endless-loop?
					break ;
				return ;
			case (0):
				//disconnect_client(); !!!!
				close(client_fd);
				return ;
			default:
				std::cout << buf << std::endl;
				command_switch(/* buf */);
				return ;
		}
	}
}

void	AServer::failure_exit(const std::string& error_msg)
{
	std::cerr << "Error: " << error_msg << ": " << std::strerror(errno) << std::endl;
	std::exit(errno); //errno?
}

//public methods
void	AServer::createTcpSocket(const std::string& ip, const int& port) //exits?
{
	struct sockaddr_in saddr;
	int	optval = 1;

	saddr.sin_family = AF_INET;
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = inet_addr(ip.c_str());

	_sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //IPPROTO_TCP?
	if (_sock_fd == -1)
		failure_exit("couldn't create socket");
	if (setsockopt(_sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) //not necassery
		failure_exit("couldn't set socket options");
	if (bind(_sock_fd, reinterpret_cast<sockaddr*>(&saddr), sizeof(struct sockaddr_in)) == -1)
		failure_exit("couldn't bind to socket");
	if (listen(_sock_fd, 1000) == -1) //1000?
		failure_exit("couldn't listen(?) to socket");
	if (fcntl(_sock_fd, F_SETFL, O_NONBLOCK) == -1)
		failure_exit("fcntl failed");
}

void	AServer::createEpoll()
{
	_ev.data.fd = _sock_fd;
	_ev.events = EPOLLIN | EPOLLET; //ECOLLET sets Edge-Triggered mode

	_epoll_fd = epoll_create1(0); //can be set to EPOLL_NONBLOCK
	if (_epoll_fd == -1)
		failure_exit("couldn't create epoll");
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _sock_fd, &_ev) == -1)
		failure_exit("epoll_ctrl failed");

	_ev.data.fd = STDIN_FILENO;
	_ev.events = EPOLLIN | EPOLLET; //needed?
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &_ev) == -1) //to get out of loop(?)
		failure_exit("epoll_ctrl failed on stdin");
	_ev.data.fd = _sock_fd;
	_ev.events = EPOLLIN | EPOLLET; //needed?
}

void	AServer::epollLoop()
{
	struct epoll_event	events[1000]; //1000?
	int 	ev_cnt;

	while (1)
	{
		//std::cout << "loop" << std::endl;
		ev_cnt = epoll_wait(_epoll_fd, events, 1000, 1000); //1000? //-1?
		if (ev_cnt == -1)
			failure_exit("epoll_wait failed"); //exits?
		for (int i = 0; i < ev_cnt; i++)
		{
			std::cout << "fd: " << events[i].data.fd << std::endl;
			if (events[i].data.fd == _sock_fd)
			{
				std::cout << "accept connection" << std::endl;
				accept_connection();
			}
			else if (events[i].data.fd == STDIN_FILENO) //fileno not allowed!!
			{
				std::string	str;
				std::cout << "stdin: ";
				std::cin >> str;
				if (str == "exit")
					return ;
			}
			//??????????v
			else if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN))) //???????????????
				close(events[i].data.fd);
			else
			{
				std::cout << "process event" << std::endl;
				process_event(events[i].data.fd); //recv
			}
		}
	}
	//close client fds in epoll?
}
