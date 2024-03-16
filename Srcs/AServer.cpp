#include "../Includes/AServer.hpp"

//con- and destructer
AServer::AServer(): _epoll_fd(-1), _sock_fd(-1)/* , _useClient(0) */{}

AServer::AServer(const std::string& name, const std::string& password): _name(name), _password(password), _epoll_fd(-1), _sock_fd(-1) {}

AServer::~AServer()
{
	for (channel_map_iter_t it = _channels.begin(); it != _channels.end(); it++)
		delete it->second;
	for (client_fd_map_iter_t it = _client_fds.begin(); it != _client_fds.end(); it++)
		delete it->second;
	if (_epoll_fd != -1)
		close(_epoll_fd);
	if (_sock_fd != -1)
		close(_sock_fd);
}

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
	// std::cout << "trying to disconnect client" << std::endl;
	if (!client)
		return ;

	std::vector<Channel *> channels = client->getAllChannels();
	for (std::vector<Channel *>::iterator it = channels.begin(); it != channels.end(); it++)
	{
		Channel *channel = (*it);
		if (!channel)
			continue ;
		int err = channel->rmClientIgnore(client, client, msg);
		if (err == DELETE_CHANNEL)
		{
			rmChannelFromMap(channel->getName());		
		}
	}
	rmClientFromMaps(client);
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
			return -1;
		case (0):
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
	if (!client)
		return;
	std::string nickname = client->getNickname();
	if(pollfds[client->_index_poll_struct].fd != 0)
	{
		pollfds[client->_index_poll_struct].fd = 0;
		pollfds[client->_index_poll_struct].events = 0;
		pollfds[client->_index_poll_struct].revents = 0;
	}

	client_fd_map_iter_t it = _client_fds.find(client->getFd());
	if (it == _client_fds.end())
		return;
	delete client;
	_client_fds.erase(it);
	//this will only be triggerd if the client didnt finish registration before losing connection
	client_name_map_iter_t it2 = _client_names.find(nickname);
	if (it2 == _client_names.end()) 
		return;
	_client_names.erase(it2);
}

void 	AServer::rmClientFromMaps(int client_fd)
{
	client_fd_map_iter_t it = _client_fds.find(client_fd);
	if (it == _client_fds.end())
		return;
	if(pollfds[it->second->_index_poll_struct].fd != 0)
	{
		pollfds[it->second->_index_poll_struct].fd = 0;
		pollfds[it->second->_index_poll_struct].events = 0;
		pollfds[it->second->_index_poll_struct].revents = 0;
	}
	delete it->second;
	_client_fds.erase(it);
	client_name_map_iter_t it2 = _client_names.find(it->second->getNickname());
	if (it2 == _client_names.end())
		return;
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
	if(!client )
		return ;
	if(client->getPipe() == true)
	{
		disconnectClient(client->getFd());
		return ;
	}
	msg += "\r\n";
	if (send(client->getFd(), msg.c_str(), msg.size(), MSG_DONTWAIT | MSG_NOSIGNAL) == -1) //MSG_DONTWAIT sets to non-block //should be nonblocking anyways because of fcntl()
	{
		if (errno == EAGAIN || errno == EWOULDBLOCK)
			return ;
        if (errno == EPIPE)
		{
			client->setPipe(true);
			/* std::cerr << "\033[0;31mWarning: BROKEN PIPE\033[0m" << std::endl; */
            disconnectClient(client->getFd());
		}
       /*  throw (std::runtime_error("send failed: ")); */ //when this happens something went fundamentally wrong
	}
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


void	AServer::accept_connection(pollfd *pollfds)
{
	struct sockaddr_in	client_addr;
	struct in_addr 		ipAddr; 
	socklen_t			client_addr_len = sizeof(client_addr);
	int 				index_poll_struct;
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
	for (index_poll_struct = 2; index_poll_struct < SERVER_MAX_CLIENTS; index_poll_struct++)		// look for a free spot in the pool struct
	{
		if (pollfds[index_poll_struct].fd == 0)
		{
			pollfds[index_poll_struct].fd = client_fd;
			pollfds[index_poll_struct].events = POLLIN | POLLHUP | POLLERR;
			break;
		}
	}
	if(index_poll_struct == SERVER_MAX_CLIENTS)
	{
		send(client_fd, ":Server full", 13, MSG_DONTWAIT | MSG_NOSIGNAL);
		close(client_fd);
		return ;
	}

	if (_client_fds.find(client_fd) != _client_fds.end())
	{
		std::cerr << "* Error: new fd already in map" << std::endl;
		return ;
	}

	ipAddr = client_addr.sin_addr;
	client_ip = inet_ntoa(ipAddr);
	if(client_ip)
		addNewClientToFdMap(client_fd, client_ip, index_poll_struct);
	else
		addNewClientToFdMap(client_fd, "9.6.9.6", index_poll_struct);
}

void AServer::resetTimerfd()
{
}

void AServer::setPollFd(pollfd &pollfd, int fd, short int events, short int revents)
{
	pollfd.fd = fd;
    pollfd.events = events;
	pollfd.revents = revents;
}

int	AServer::createpoll()
{
	setPollFd(pollfds[0], _sock_fd, POLLIN | POLLPRI | POLLHUP | POLLERR, 0);
	setPollFd(pollfds[1], 0, POLLIN, 0);
	for (int i = 2; i < SERVER_MAX_CLIENTS; i++)
		setPollFd(pollfds[i], 0, 0, 0);
	return (0);
}

void	AServer::pollLoop()
{
	std::string	stdin_input = "run";
	Client 		*client;

	while (stdin_input != "exit" && stdin_input != "Exit")
	{
		protectedPoll(1000);
		if ((pollfds[1].fd == 0 && pollfds[1].revents & POLLIN))
		{
			pollPrintClientsWho(stdin_input);
			pollfds[1].revents = 0;
		}
		if (pollfds[0].revents & POLLIN)
		{
			accept_connection(pollfds);
			pollfds[1].revents = 0;
		}
		for (int i = 2; i < SERVER_MAX_CLIENTS; i++)
		{
			client = getClient(pollfds[i].fd);
			if((pollfds[i].revents & POLLHUP) || (pollfds[i].revents & POLLERR) || (client && client->getPipe() == true)) // client disconect or broken pipe
				disconnectClient(pollfds[i].fd);
			else if ((pollfds[i].fd > 0) && (pollfds[i].revents & POLLIN))
				if(process_event(pollfds[i].fd) < 1)
					disconnectClient(pollfds[i].fd);
		}
	}
}

void	AServer::protectedPoll(int timeout)
{
	int		poll_return;

	poll_return = poll(pollfds, static_cast<nfds_t>(SERVER_MAX_CLIENTS), timeout); 			// + 2 for the stdin watching and the poll also needs one
	if (poll_return == -1)
		throw (std::runtime_error("poll: "));
}


void	AServer::pollPrintClientsWho(std::string &stdin_input)
{
	std::getline(std::cin, stdin_input);
	if(stdin_input == "who")
	{
		std::cout << "+" << std::setfill ('-') << std::setw (40) << "+" << std::setfill (' ') << std::endl;
		std::cout << "|" << std::setw(40) << "|" << std::endl;
		std::cout << "|" << std::setw(11) << _client_fds.size() << " CLIENTS CONNECTED" << std::setw(11) << "|" << std::endl;
		std::cout << "|" << std::setw(40) << "|" << std::endl;
		std::cout << "+" << std::setfill ('-') << std::setw (40) << "+" << std::setfill (' ') << std::endl;
		std::cout << "|" << std::setw(20) << "FDS" << std::setw(20) << "|" << std::endl;
		std::cout << "+" << std::setfill ('-') << std::setw (40) << "+" << std::setfill (' ') << std::endl;
		int i = 0;
		for(client_fd_map_iter_t itr = _client_fds.begin(); itr != _client_fds.end() && i < 10; itr++)
		{
			std::cout << itr->first;
			if(i < 9)
				std::cout << ", ";
			else
				std::cout << "...";
			i++;
		}
		std::cout << std::endl;
	}
}
