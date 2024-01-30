#include "../Includes/AServer.hpp"

//con- and destructer
AServer::AServer(): _epoll_fd(-1), _sock_fd(-1){}

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
void	AServer::accept_connection()
{
	struct sockaddr_in	client_addr;
	struct epoll_event	ev_temp;
	socklen_t			client_addr_len = sizeof(client_addr);
	struct in_addr 		ipAddr; 
	char				client_ip_buf[INET_ADDRSTRLEN];
	int					client_fd;

	while (1)
	{
		client_fd = accept(_sock_fd, reinterpret_cast<struct sockaddr*>(&client_addr), &client_addr_len);
		if (client_fd == -1) //better protection? //should we exit?
		{
			if (errno != EAGAIN && errno != EWOULDBLOCK) //if not
				std::cerr << "Error: accept failed" << std::endl;
			return ;
		}
		memset(client_ip_buf, '\0', INET_ADDRSTRLEN);
		ipAddr = client_addr.sin_addr;
		inet_ntop(AF_INET, &ipAddr, client_ip_buf, INET_ADDRSTRLEN);
		std::cout << "Client IP: " << client_ip_buf << "!\n";
		ev_temp.data.fd = client_fd;
		ev_temp.events = EPOLLIN | EPOLLET; //needs to be in loop? //niki says it could be changed in epoll_ctl() so just to be save
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
		addNewClientToFdMap(client_fd, client_ip_buf); //allocatoes the client object
		std::cout << "Added new Client to Fd-Map!" << std::endl;
	}
}


void	AServer::disconnect_client(const int& client_fd)
{
	Client *client = getClient(client_fd);
	(void)(client);
	// disconnectClient(Client *client, "lost connection, bye bye, see you in the AfterLife");
}

// void	AServer::disconnectClient(Client *client, const std::string& msg)
// {
	// std::vector<Channel *> channels = client->getAllChannels();
	// for (std::vector<Channel *>::iterator it = channels.begin(); it != channels.end(); it++)
	// {
		// Channel *channel = (*it); 
		// if (!channel) //necessary? checking if the channel is in map or null seems overkill, since this case should never happen
			// continue ;
		/* client->leaveChannel(channel); //unecessary ?? he will leave entire server */
		// int err = channel->rmClient(client, msg);
		// if (err == -1) //exchange -1 with CHANNEL_DEAD, and use rmClient with a message
			// rmChannelFromMap(channel->getName());		
	// }
	// rmClientFromMaps(client);
// }

void	AServer::process_event(const int& client_fd)
{
	char	buf[513];
	int		bytes_recieved = -1; //better name?

	Client *sender = _client_fds.find(client_fd)->second;

	
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
			//disconnect_client(client_fd);
			close(client_fd);
			return ;
		case (1):
			return ;
		default:
			std::stringstream	sstream(buf);
			std::string str;
			while(splitMsg(sstream, str)) //we have to do a while loop cuz "CMD\nCMD1\nCMD3\n"
			{
				sender->loadMsgBuf(str); //take str as reference
				str = sender->readMsgBuf();
				if (!str.empty())
					command_switch(sender, str); //what if fd is not in map?
			}
	}
}

void	AServer::failure_exit(const std::string& error_msg)
{
	(void)(error_msg);
	// std::cerr << "Error: " << error_msg << ": " << std::strerror(errno) << std::endl;
	std::exit(errno); //errno?
}

void	AServer::addNewChannelToMap(Client *sender, const std::string& channel_name)
{
	Channel	*temp_channel = new Channel(sender, channel_name); //protect new?
	std::pair<std::string, Channel*>	pair(channel_name, temp_channel);
	_channels.insert(pair); //check if already in map!???
}
void	AServer::addClientToNameMap(std::string user_name, const int& client_fd) //add more?
{
	Client	*temp_client = _client_fds.find(client_fd)->second;
	std::pair<std::string, Client*>	pair(user_name, temp_client);
	_client_names.insert(pair);
}
void	AServer::addNewClientToFdMap(const int& client_fd, const std::string& client_ip)
{
	Client	*temp_client = new Client(client_fd, client_ip); //protect new?
	std::pair<int, Client*>	pair(client_fd, temp_client);
	_client_fds.insert(pair); //check if already in map!???
}

void 	AServer::rmClientFromMaps(Client *client) //necessary? we always have fd if we have client
{
	if (!client) return;

	client_fd_map_iter_t it = _client_fds.find(client->getFd());
	if (it == _client_fds.end())
		return;
	client_name_map_iter_t it2 = _client_names.find(client->getNickname());
	if (it2 == _client_names.end()) //this should never be triggered
		return;
	_client_fds.erase(it);
	_client_names.erase(it2);
	delete client;
}

void 	AServer::rmClientFromMaps(int client_fd)
{
	client_fd_map_iter_t it = _client_fds.find(client_fd);
	if (it == _client_fds.end())
		return;
	client_name_map_iter_t it2 = _client_names.find(it->second->getNickname());
	if (it2 == _client_names.end()) //this should never be triggered
		return;
	delete it->second;
	_client_fds.erase(it);
	_client_names.erase(it2);
}

void 	AServer::rmChannelFromMap(const std::string& channel_name)
{
	channel_map_iter_t it = _channels.find(channel_name);
	if (it == _channels.end())
		return ;
	delete it->second;
	_channels.erase(it);
}

// struct sockaddr_in addr;
// struct in_addr ipAddr = addr.sin_addr;
// char str[INET_ADDRSTRLEN];
// inet_ntop(AF_INET, &ipAddr, str, INET_ADDRSTRLEN);
// std::cout << "inet: " << str << "|\n";

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
	std::string			str = "run";
	int 				ev_cnt;

	while (str != "exit")
	{
		//std::cout << "loop" << std::endl;
		ev_cnt = epoll_wait(_epoll_fd, events, 1000, 1000); //1000? //-1?
		if (ev_cnt == -1)
			failure_exit("epoll_wait failed"); //exits?
		for (int i = 0; i < ev_cnt; i++)
		{
			std::cout << "fd: " << events[i].data.fd << std::endl;			
			if (events[i].data.fd == _sock_fd)
				accept_connection();
			else if (events[i].data.fd == STDIN_FILENO)
				std::cin >> str;
			else if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN))) //???????????????
			{
				close(events[i].data.fd); //Irc::QUIT() with a "client died" message ?!
				//clientDied() QUIT(); //writing own function similar to QUIT? it has to resolve the fd to the client
			}
			else
				process_event(events[i].data.fd); //recv
		}
	}
	//close client fds in epoll?
}

AServer::client_fd_map_const_it		AServer::getClientIter(int fd) const {return _client_fds.find(fd);}
AServer::client_name_map_const_it	AServer::getClientIter(const std::string& name) const {return _client_names.find(name);}
AServer::channel_map_const_it		AServer::getChannelIter(const std::string& name) const {return _channels.find(name);}

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
