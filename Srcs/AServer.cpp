#include "../Includes/AServer.hpp"

//con- and destructer
AServer::AServer(): _epoll_fd(-1), _sock_fd(-1){}

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
		if (client_fd == -1) //better protection? //should we exit?
		{
			if (errno != EAGAIN && errno != EWOULDBLOCK) //if not
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
		if (_client_fds.find(client_fd) != _client_fds.end())
			std::cerr << "* Error: new fd already in map" << std::endl;
		_client_fds[client_fd] = NULL;
	}
}

void	AServer::process_event(const int& client_fd)
{
	char	buf[513];
	int		bytes_recieved = -1; //better name?

	while (1)
	{
		memset(buf, '\0', 513);
		bytes_recieved = recv(client_fd, buf, sizeof(buf) - 1, 0);
		switch (bytes_recieved)
		{
			case (-1):
				/* if (errno == EAGAIN || errno == EWOULDBLOCK) //leave it in? //potential endless-loop?
					break ; */ 			//loops when ctrl-D is pressed and waits for enter from same client
				std::cerr << "Error: couldn't recieve data :" << std::strerror(errno) << std::endl;
				return ;
			case (0):
				//disconnect_client(); !!!!
				close(client_fd);
				return ;
			default:
				std::cout << "buf: " << buf << std::endl << std::endl;
				command_switch(_client_fds.find(client_fd)->second, buf, client_fd); //what if fd is not in map?
				return ;
		}
	}
}

void	AServer::failure_exit(const std::string& error_msg)
{
	std::cerr << "Error: " << error_msg << ": " << std::strerror(errno) << std::endl;
	std::exit(errno); //errno?
}

void	AServer::addNewPair(Client *sender, const std::string& channel_name)
{
	Channel	*temp_channel = new Channel(sender, channel_name); //protect new?
	std::pair<std::string, Channel*>	pair(channel_name, temp_channel);
	_channels.insert(pair);
}
void	AServer::addNewPair(std::string user_name, const int& client_fd) //add more?
{
	Client	*temp_client = new Client(client_fd); //protect new?
	std::pair<std::string, Client*>	pair(user_name, temp_client);
	_client_names.insert(pair);
}
void	AServer::addNewPair(const int& client_fd)
{
	Client	*temp_client = new Client(client_fd); //protect new?
	std::pair<int, Client*>	pair(client_fd, temp_client);
	_client_fds.insert(pair);
}

struct addrinfo* AServer::getIpAdressToBind(const int& port) // NOT needed CRY.... !!!???
{
	struct addrinfo hints, *result, *ptr;
	int status;
	char ipstr[INET_ADDRSTRLEN];

	std::stringstream ss;
	ss << port;
	std::string portSTR = ss.str();
	std::cout << "PORT: " << portSTR << std::endl;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET; // AF_INET or AF_INET6 to force version
	hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // to return a socket suitable for binding a socket for accepting 
	if ((status = getaddrinfo(NULL, portSTR.c_str(), &hints, &result)) != 0)
    {
		// std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl; // not allowed i think
		failure_exit("failed to get addrinfo");
	}
    
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) 
	{
        if (ptr->ai_family == AF_INET) //Only process IPv4 addresses
		{ 
            struct sockaddr_in *ipv4 = reinterpret_cast<struct sockaddr_in*>(ptr->ai_addr);
            inet_ntop(ptr->ai_family, &(ipv4->sin_addr), ipstr, sizeof(ipstr)); //Convert the IPv4 address to a human-readable form
            std::cout << "IPv4 Address: " << ipstr << std::endl;
        }
    }
	return result;
}

//public methods
void	AServer::createTcpSocket(const int& port) //exits?
{
	int	optval = 1;

	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET; 
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = INADDR_ANY;

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
	_ev.events = EPOLLIN | EPOLLET; //ECOLLET sets Edge-Triggered mode ?? whats the differnce again with level triggered write it down somewhere

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
			else if (events[i].data.fd == STDIN_FILENO)
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
