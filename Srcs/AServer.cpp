#include "../Includes/AServer.hpp"

//con- and destructer
AServer::AServer(): _epoll_fd(-1), _sock_fd(-1), _useClient(0){}

AServer::AServer(const std::string& name, const std::string& password): _name(name), _password(password), _epoll_fd(-1), _sock_fd(-1) {}
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
	/* for (client_name_map_iter_t it = _client_names.begin(); it != _client_names.end(); it++)
		delete it->second; */
	for (client_fd_map_iter_t it = _client_fds.begin(); it != _client_fds.end(); it++)
		delete it->second;
	if (_epoll_fd != -1)
		close(_epoll_fd);
	if (_sock_fd != -1)
		close(_sock_fd);
}

//private methods
/* 
//private methods
void	AServer::accept_connection()
{
	struct sockaddr_in	client_addr;
	struct epoll_event	ev_temp;
	socklen_t			client_addr_len = sizeof(client_addr);
	struct in_addr 		ipAddr; 
	char				*client_ip;
	int					client_fd;

	client_fd = accept(_sock_fd, reinterpret_cast<struct sockaddr*>(&client_addr), &client_addr_len);
	if (client_fd == -1)
	{
		std::cerr << "Error: accept failed :" << std::strerror(errno) << std::endl;
		if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EAGAIN || errno == EWOULDBLOCK || errno == ECONNABORTED || errno == EFAULT || errno == EINTR || errno == EMFILE || errno == ENFILE || errno == EPERM)
			return ;
		throw (std::runtime_error("accept failed: "));//when this happens something went fundamentally wrong
	}
	ipAddr = client_addr.sin_addr;
	client_ip = inet_ntoa(ipAddr);
	if(!client_ip)
	{
		client_ip[0] = '0';
		client_ip[1] = '\0'; 
	}
	std::cout << "Client IP: " << client_ip << "!\n";
	ev_temp.data.fd = client_fd;
	ev_temp.events = EPOLLIN | EPOLLET;
	fcntl(client_fd, F_SETFL, O_NONBLOCK);
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &ev_temp) == -1)
	{
		std::cerr << "Error: epoll_ctl failed in accept_connection" << std::endl;
		return ;
	}
	if (_client_fds.find(client_fd) != _client_fds.end())
	{
		std::cerr << "* Error: new fd already in map" << std::endl;
		return ;
	}
	addNewClientToFdMap(client_fd, client_ip); //allocatoes the client object
	std::cout << "Added new Client to Fd-Map!" << std::endl;
} */


void	AServer::disconnectClient(const int& client_fd) //for case 0:
{
	Client	*sender = getClient(client_fd);

	if (sender == NULL)
	{
		std::cout << "sender is null in diconnectClient" << std::endl;
		return ;
	}
	disconnectClient(sender, ":" + sender->getPrefix() + " QUIT :" + "lost connection, bye bye, see you in the AfterLife");
}

void	AServer::disconnectClient(Client *client, const std::string& msg)
{
	if (!client)
		return ;

	std::vector<Channel *> channels = client->getAllChannels();
	for (std::vector<Channel *>::iterator it = channels.begin(); it != channels.end(); it++)
	{
		Channel *channel = (*it);
		if (!channel)
			continue ;
		/* std::cout << "Removing client from channel: " << channel->getName() << std::endl; */
		int err = channel->rmClientIgnore(client, client, msg);
		if (err == DELETE_CHANNEL)
		{
			rmChannelFromMap(channel->getName());		
			/* std::cout << "Deleted Channel from Map!" << std::endl; */
		}
	}
	rmClientFromMaps(client);
	/* std::cout << "deleted client from maps" << std::endl; */
}

int	AServer::process_event(const int& client_fd)
{
	char	buf[513];
	int		bytes_recieved = -1;

	Client *sender = getClient(client_fd);

	std::memset(buf, '\0', 513);
	bytes_recieved = recv(client_fd, buf, 512 * sizeof(char), 0);
	switch (bytes_recieved)
	{
		case (-1):
			std::cout << "case -1" << std::endl;
			/* std::cout << "Warning: read faild" << std::endl;
			disconnectClient(client_fd);
			break ; */
			/* if (errno == EAGAIN || errno == EWOULDBLOCK) */
			/* throw (std::runtime_error("couldn't recieve data: ")); */
			return -1;
		case (0):
			std::cout << "case 0" << std::endl;
			/* disconnectClient(client_fd); */ //ctrl+c at netcat
			/* sender->loadMsgBuf(buf); */
			return 0;
		default:
			std::stringstream	sstream(buf);
			std::string str;
			while(splitMsg(sstream, str) && sender != NULL) //we have to do a while loop cuz "CMD\nCMD1\nCMD3\n"
			{
				sender->loadMsgBuf(str);
				str = sender->readMsgBuf();
				if (!str.empty())
					command_switch(sender, str);
				sender = getClient(client_fd);
			}
	}
	return (bytes_recieved);
}

int	AServer::printErrorReturn(const std::string& error_msg)
{
	std::cerr << "Error: " << error_msg << std::endl;
	return (-1);
}

void	AServer::addNewChannelToMap(Client *sender, std::string channel_name)
{
	for(size_t i = 0; i < channel_name.size(); i++)
		channel_name[i] = std::tolower(channel_name[i]);
	Channel	*temp_channel = new Channel(sender, channel_name);
	std::pair<std::string, Channel*>	pair(channel_name, temp_channel);
	_channels.insert(pair);
}
void	AServer::addClientToNameMap(std::string user_name, const int& client_fd)
{
	Client	*temp_client = _client_fds.find(client_fd)->second;
	std::pair<std::string, Client*>	pair(user_name, temp_client);
	_client_names.insert(pair);
}
void	AServer::addNewClientToFdMap(const int& client_fd, const std::string& client_ip, int index_poll_struct)
{
	Client	*temp_client = new Client(client_fd, client_ip);
	temp_client->_index_poll_struct = index_poll_struct;
	std::pair<int, Client*>	pair(client_fd, temp_client);
	_client_fds.insert(pair);
}

void 	AServer::rmClientFromNameMap(const std::string& nick_name)
{
	client_name_map_iter_t it = _client_names.find(nick_name);
	if (it == _client_names.end())
		return ;
	_client_names.erase(it);
}

void 	AServer::rmClientFromMaps(Client *client)
{
	if (!client) return;

	client_fd_map_iter_t it = _client_fds.find(client->getFd());
	client_name_map_iter_t it2 = _client_names.find(client->getNickname());
	delete client;
	if(pollfds[client->_index_poll_struct].fd != 0)
	{
		pollfds[client->_index_poll_struct].fd = 0;
		pollfds[client->_index_poll_struct].events = 0;
		pollfds[client->_index_poll_struct].revents = 0;
		_useClient--;
	}
	
	if (it == _client_fds.end())
		return;

	_client_fds.erase(it);
	
	//this will only be triggerd if the client didnt finish registration before losing connection
	if (it2 == _client_names.end()) 
		return;

	_client_names.erase(it2);
}

void 	AServer::rmClientFromMaps(int client_fd)
{
	client_fd_map_iter_t it = _client_fds.find(client_fd);
	if (it == _client_fds.end())
		return;
	client_name_map_iter_t it2 = _client_names.find(it->second->getNickname());
	if (it2 == _client_names.end())
		return;
	delete it->second;
	_client_fds.erase(it);
	_client_names.erase(it2);
}

void 	AServer::rmChannelFromMap(std::string channel_name)
{
	for(size_t i = 0; i < channel_name.size(); i++)
		channel_name[i] = std::tolower(channel_name[i]);
	channel_map_iter_t it = _channels.find(channel_name);
	if (it == _channels.end())
		return ;
	delete it->second;
	_channels.erase(it);
}


//public methods
int	AServer::createTcpSocket(const int& port)
{
	int	optval = 1;

	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET; 
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = INADDR_ANY;

	_sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_sock_fd == -1)
		return (printErrorReturn("couldn't create socket"));

	if (setsockopt(_sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
		return (printErrorReturn("couldn't set socket options"));
	//SO_REUSEADDR == to reuse the same socked adress. Otherwise server cloud not instaned bind socked after exit.

	if (bind(_sock_fd, reinterpret_cast<sockaddr*>(&saddr), sizeof(struct sockaddr_in)) == -1)
		return (printErrorReturn("couldn't bind to socket"));

	if (listen(_sock_fd, 1000) == -1)
		return (printErrorReturn("couldn't listen to socket"));

	if (fcntl(_sock_fd, F_SETFL, O_NONBLOCK) == -1)
		return (printErrorReturn("fcntl failed"));

	return (0);
}

/* int	AServer::createEpoll()
{
	_kevent_fd = kqueue();
	EV_SET(change_event, _sock_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
	if (kevent(_kevent_fd, change_event, 1, NULL, 0, NULL) == -1)
	{
		throw (std::runtime_error("kevent create failed: "));
	}
	return (0);
} */

int	AServer::createEpoll()
{
	/* _ev.data.fd = _sock_fd;
	_ev.events = EPOLLIN | EPOLLET;
	_epoll_fd = epoll_create1(0);
	if (_epoll_fd == -1)
		return (printErrorReturn("couldn't create epoll"));
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _sock_fd, &_ev) == -1)
		return (printErrorReturn("epoll_ctrl failed"));

	_ev.data.fd = STDIN_FILENO;
	_ev.events = EPOLLIN | EPOLLET;
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &_ev) == -1)
		return (printErrorReturn("epoll_ctrl failed on stdin")); */
	return (0);
}

/* void	AServer::epollLoop()
{
	struct epoll_event	event[1000];
	std::string			str = "run";
	int 				ev_cnt;

	while (str != "exit")
	{
		std::cout << "epoll loop" << std::endl;
		ev_cnt = epoll_wait(_epoll_fd, events, 1000, 1000);
		if (ev_cnt == -1)
			throw (std::runtime_error("epoll_wait failed: "));
		for (int i = 0; i < ev_cnt; i++)
		{
			std::cout << "fd: " << events[i].data.fd << std::endl;			
			if (events[i].data.fd == _sock_fd)
				accept_connection();
			else if (events[i].data.fd == STDIN_FILENO)
				std::cin >> str;
			else if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN)))
			{
				std::cerr << "Error: broken event" << std::endl;
				disconnectClient(events[i].data.fd);
			}
			else
				process_event(events[i].data.fd); //recv
		}
	}
} */

void	AServer::accept_connection(pollfd *pollfds)
{
	struct sockaddr_in	client_addr;
	/* struct epoll_event	ev_temp; */
	socklen_t			client_addr_len = sizeof(client_addr);
	struct in_addr 		ipAddr; 
	char				*client_ip;
	int					client_fd;

	client_fd = accept(_sock_fd, reinterpret_cast<struct sockaddr*>(&client_addr), &client_addr_len);
	if (client_fd == -1)
	{
		std::cerr << "Error: accept failed :" << std::strerror(errno) << std::endl;
		/* if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EAGAIN || errno == EWOULDBLOCK || errno == ECONNABORTED || errno == EFAULT || errno == EINTR || errno == EMFILE || errno == ENFILE || errno == EPERM)
			return ; */
		throw (std::runtime_error("accept failed: "));//when this happens something went fundamentally wrong
	}
	/* std::cerr << "new fd:" << client_fd << std::endl; */
	int index_poll_struct = -1;
	for (index_poll_struct = 2; index_poll_struct < MAX_CLIENTS; index_poll_struct++)
	{
		if (pollfds[index_poll_struct].fd == 0)
		{
			/* std::cerr << "i:" << index_poll_struct << std::endl; */
			pollfds[index_poll_struct].fd = client_fd;
			pollfds[index_poll_struct].events = POLLIN | POLLHUP;
			_useClient++;
			break;
		}
	}

	ipAddr = client_addr.sin_addr;
	client_ip = inet_ntoa(ipAddr);
	if(!client_ip)
	{
		client_ip[0] = '0';
		client_ip[1] = '\0'; 
	}

	/* std::cout << "Client IP: " << client_ip << "!\n"; */


	if (_client_fds.find(client_fd) != _client_fds.end())
	{
		std::cerr << "* Error: new fd already in map" << std::endl;
		return ;
	}
	addNewClientToFdMap(client_fd, client_ip, index_poll_struct); //allocatoes the client object
	/* std::cout << "Added new Client to Fd-Map!" << std::endl; */
}

#include <iomanip>

void	AServer::epollLoop()
{
	struct pollfd pollfds[MAX_CLIENTS + 2];
    pollfds[0].fd = _sock_fd;
    pollfds[0].events = POLLIN | POLLPRI | POLLHUP;
	pollfds[0].revents = 0;

	pollfds[1].fd = 0;
   	pollfds[1].events = POLLIN;
	pollfds[1].revents = 0;
	_useClient = 0;

	for (int i = 2; i < MAX_CLIENTS; i++)
	{
		pollfds[i].fd = 0;
		pollfds[i].events = POLLIN | POLLHUP;
		pollfds[i].revents = 0;
	}

	std::string			str = "run";
	int 				poll_return;

	while (str != "exit" && str != "Exit")
	{
		poll_return = poll(pollfds, _useClient + 2, 1000);
        if (poll_return == -1)
            throw (std::runtime_error("poll failed: "));
		if(poll_return == 0)
		{
			/* std::cout << "time_out" << std::endl; */
			for(client_fd_map_iter_t itr = _client_fds.begin(); itr != _client_fds.end(); itr++)
				protectedSend(itr->second, "ping");
		}
		if ((pollfds[1].fd == 0 && pollfds[1].revents & POLLIN))
		{
			std::getline(std::cin, str);
			if(str == "who")
			{
				std::cout << "+" << std::setfill ('-') << std::setw (40) << "+" << std::setfill (' ') << std::endl;
				std::cout << "|" << std::setw(40) << "|" << std::endl;
				std::cout << "|" << std::setw(11) << _client_fds.size() << "  CLIENTS CONECTED" << std::setw(11) << "|" << std::endl;
				std::cout << "|" << std::setw(40) << "|" << std::endl;
				std::cout << "+" << std::setfill ('-') << std::setw (40) << "+" << std::setfill (' ') << std::endl;
				std::cout << "|" << std::setw(20) << "FDS" << std::setw(20) << "|" << std::endl;
				std::cout << "+" << std::setfill ('-') << std::setw (40) << "+" << std::setfill (' ') << std::endl;
				int i = 0;
				for(client_fd_map_iter_t itr = _client_fds.begin(); itr != _client_fds.end(); itr++)
				{
					std::cout << itr->first;
					if(i != 10)
						std::cout << ", ";
					else
					{
						std::cout << std::endl;
						i = 0;
					}
					i++;
				}
				std::cout << std::endl << _useClient << std::endl;
			}
		}
		if (pollfds[0].revents & POLLIN)
			accept_connection(pollfds);
		for (int i = 2; i < MAX_CLIENTS; i++)
		{
			Client *client = getClient(pollfds[i].fd);
			if((client && client->getPipe() == true) || (pollfds[i].revents & POLLHUP))
			{
				std::cout << "broken pipe event in poll" << std::endl;
				disconnectClient(pollfds[i].fd);
				if(pollfds[i].fd != 0)
				{
					pollfds[i].fd = 0;
					pollfds[i].events = 0;
					pollfds[i].revents = 0;
					_useClient--;
				}
			}
			else if ((pollfds[i].fd > 0) && (pollfds[i].revents & POLLIN))
			{
				/* std::cout << "poll event" << std::endl; */
				if(process_event(pollfds[i].fd) < 1)
				{
					disconnectClient(pollfds[i].fd);
					if(pollfds[i].fd != 0)
					{
						pollfds[i].fd = 0;
						pollfds[i].events = 0;
						pollfds[i].revents = 0;
						_useClient--;
					}
				}
			}
		}
	}
}

AServer::client_fd_map_const_it		AServer::getClientIter(int fd) const {return _client_fds.find(fd);}
AServer::client_name_map_const_it	AServer::getClientIter(const std::string& name) const {return _client_names.find(name);}
AServer::channel_map_const_it		AServer::getChannelIter(std::string name) const 
{
	for(size_t i = 0; i < name.size(); i++)
		name[i] = std::tolower(name[i]);
	return _channels.find(name);
}

Channel*	AServer::getChannel(const std::string& name) const
{
	channel_map_const_it it = getChannelIter(name);
	if (it == _channels.end())
		return NULL;
	return it->second;
}

Client*		AServer::getClient(int fd) const
{
	client_fd_map_const_it it = getClientIter(fd);
	if (it == _client_fds.end())
		return NULL;
	return it->second;
}

Client*		AServer::getClient(const std::string& name) const
{
	client_name_map_const_it it = getClientIter(name);
	if (it == _client_names.end())
		return NULL;
	return it->second;
}

void	AServer::protectedSend(Client *client, std::string msg)
{
	msg += "\r\n";
	if(!client || client->getPipe() == true)
		return ;
	if (send(client->getFd(), msg.c_str(), msg.size(), MSG_DONTWAIT | MSG_NOSIGNAL) == -1) //MSG_DONTWAIT sets to non-block //should be nonblocking anyways because of fcntl()
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return ;
        if (errno == EPIPE)
		{
			client->setPipe(true);
			std::cerr << "\033[0;31mWarning: BROKEN PIPE\033[0m" << std::endl;
            return disconnectClient(client->getFd());
		}
       /*  throw (std::runtime_error("send failed: ")); */ //when this happens something went fundamentally wrong
	}
}
