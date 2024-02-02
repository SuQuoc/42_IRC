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

	client_fd = accept(_sock_fd, reinterpret_cast<struct sockaddr*>(&client_addr), &client_addr_len);
	if (client_fd == -1)
	{
		std::cerr << "Error: accept failed :" << std::strerror(errno) << std::endl;
		if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EAGAIN || errno == EWOULDBLOCK || errno == ECONNABORTED || errno == EFAULT || errno == EINTR || errno == EMFILE || errno == ENFILE || errno == EPERM)
			return ;
		throw (std::logic_error("accept failed: "));//when this happens something went fundamentally wrong
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


void	AServer::disconnectClient(const int& client_fd) //for case 0:
{
	disconnectClient(getClient(client_fd), "lost connection, bye bye, see you in the AfterLife");
}

void	AServer::disconnectClient(Client *client, const std::string& msg)
{
	if (!client)
		return ;
	
	std::vector<Channel *> channels = client->getAllChannels();
	for (std::vector<Channel *>::iterator it = channels.begin(); it != channels.end(); it++)
	{
		Channel *channel = (*it);
		if (!channel) //necessary? checking if the channel is in map or null seems overkill, since this case should never happen
			continue ;
		std::cout << "Removing client from channel: " << channel->getName() << std::endl;
		int err = channel->rmClientTest(client, client, msg);
		if (err == DELETE_CHANNEL) //exchange -1 with CHANNEL_DEAD, and use rmClient with a message
		{
			rmChannelFromMap(channel->getName());		
			std::cout << "Deleted Channel from Map!" << std::endl;
		}
	}
	rmClientFromMaps(client);
}

void	AServer::process_event(const int& client_fd)
{
	char	buf[513];
	int		bytes_recieved = -1; //better name?

	Client *sender = getClient(client_fd);

	
	memset(buf, '\0', 513);
	bytes_recieved = recv(client_fd, buf, sizeof(buf) - 1, 0);
	switch (bytes_recieved)
	{
		case (-1):
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				return ;
			throw (std::logic_error("couldn't recieve data: ")); //when this happens something went fundamentally wrong
		case (0):
			std::cerr << "DISCONNECTING CLIENT here????" << std::endl;
			disconnectClient(client_fd); //ctrl+c at netcat
			return ;
		default:
			std::stringstream	sstream(buf);
			std::string str;
			while(splitMsg(sstream, str) && sender != NULL) //we have to do a while loop cuz "CMD\nCMD1\nCMD3\n"
			{
				sender->loadMsgBuf(str); //take str as reference
				str = sender->readMsgBuf();
				if (!str.empty())
					command_switch(sender, str); //what if fd is not in map?
				sender = getClient(client_fd);
			}
	}
}

int	AServer::printErrorReturn(const std::string& error_msg)
{
	std::cerr << "Error: " << error_msg << std::endl;
	return (-1);
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

void 	AServer::rmClientFromNameMap(const std::string& nick_name)
{
	client_name_map_iter_t it = _client_names.find(nick_name);
	if (it == _client_names.end())
		return ;
	_client_names.erase(it);
}

void 	AServer::rmClientFromMaps(Client *client) //necessary? we always have fd if we have client
{
	if (!client) return;

	client_fd_map_iter_t it = _client_fds.find(client->getFd());
	client_name_map_iter_t it2 = _client_names.find(client->getNickname());
	if (it == _client_fds.end())
		return;

	_client_fds.erase(it);
	delete client;
	
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


//public methods
int	AServer::createTcpSocket(const int& port)
{
	int	optval = 1;

	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET; 
	saddr.sin_port = htons(port);
	saddr.sin_addr.s_addr = INADDR_ANY;
	_sock_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //IPPROTO_TCP?
	if (_sock_fd == -1)
		return (printErrorReturn("couldn't create socket"));
	if (setsockopt(_sock_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) //not necassery
		return (printErrorReturn("couldn't set socket options"));
	if (bind(_sock_fd, reinterpret_cast<sockaddr*>(&saddr), sizeof(struct sockaddr_in)) == -1)
		return (printErrorReturn("couldn't bind to socket"));
	if (listen(_sock_fd, 1000) == -1) //1000?
		return (printErrorReturn("couldn't listen(?) to socket"));
	if (fcntl(_sock_fd, F_SETFL, O_NONBLOCK) == -1)
		return (printErrorReturn("fcntl failed"));
	return (0);
}

int	AServer::createEpoll()
{
	_ev.data.fd = _sock_fd;
	_ev.events = EPOLLIN | EPOLLET; //ECOLLET sets Edge-Triggered mode ?? whats the differnce again with level triggered write it down somewhere
	_epoll_fd = epoll_create1(0); //can be set to EPOLL_NONBLOCK
	if (_epoll_fd == -1)
		return (printErrorReturn("couldn't create epoll"));
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, _sock_fd, &_ev) == -1)
		return (printErrorReturn("epoll_ctrl failed"));

	_ev.data.fd = STDIN_FILENO;
	_ev.events = EPOLLIN | EPOLLET; //needed?
	if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &_ev) == -1) //to get out of loop(?)
		return (printErrorReturn("epoll_ctrl failed on stdin"));
	_ev.data.fd = _sock_fd;
	_ev.events = EPOLLIN | EPOLLET; //needed?
	return (0);
}

void	AServer::epollLoop()
{
	struct epoll_event	events[1000]; //1000?
	std::string			str = "run";
	int 				ev_cnt;

	while (str != "exit")
	{
		ev_cnt = epoll_wait(_epoll_fd, events, 1000, 1000); //1000?
		if (ev_cnt == -1)
			throw (std::logic_error("epoll_wait failed: "));
		for (int i = 0; i < ev_cnt; i++)
		{
			std::cout << "fd: " << events[i].data.fd << std::endl;			
			if (events[i].data.fd == _sock_fd)
				accept_connection();
			else if (events[i].data.fd == STDIN_FILENO)
				std::cin >> str;
			else if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN))) //???????????????
			{
				// dont know when this is getting triggered
				// maybe internet loss? wouldnt make sense
				std::cerr << "DISCONNECTING CLIENT here????" << std::endl;
				disconnectClient(events[i].data.fd); //this case wont be triggered by ctrl-c at netcat
				// close(events[i].data.fd);
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
