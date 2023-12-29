

# include "irc.hpp"

const int MAX_EVENTS = 10;
const int MAX_CLIENTS = 10;

int *servFdPTR; // only until we do a class for server
int *epollFDPTR;



std::string get_ip_str(const struct sockaddr *sa)
{
    char client_ip[INET6_ADDRSTRLEN];
    std::string ip_str;

    switch(sa->sa_family) 
    {
        case AF_INET:
            inet_ntop(AF_INET, &(((struct sockaddr_in *)sa)->sin_addr), client_ip, sizeof(client_ip));
            break;
        case AF_INET6:
            inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)sa)->sin6_addr), client_ip, sizeof(client_ip));
            break;
        default:
            ip_str.assign("Unknown AF", 0, sizeof(client_ip));
            return ip_str;
    }
    ip_str.assign(client_ip, 0, sizeof(client_ip));
    return std::string(client_ip);
}


void handle_SIGINT(int sig)
{
    close(*servFdPTR);
    close(*epollFDPTR);
    _exit(0);
}

int main(int argc, char **argv) 
{
    if (argc != 3) {
	    std::cerr << "Usage: ./ircserv \"port (6667)\" \"password\"\n";
	    return 1;
	}

    struct sigaction sa;
    sa.sa_handler = &handle_SIGINT;
    sa.sa_flags = SA_RESTART; // library functions resume, function acts normally of SIGNAL was send during its execution.
    sigaction(SIGINT, &sa, NULL); // last para holds old signal handler

    const std::string _port = argv[1];
	const std::string _pw = argv[2];

    int port = convertPort(_port);
    if (port == -1)
    {
        perror("portconvert");
    }

    struct addrinfo hints, *result, *p;
	int status;
	char ipstr[INET6_ADDRSTRLEN];
	
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
	hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // 
	if ((status = getaddrinfo("10.13.4.6", _port.c_str(), &hints, &result)) != 0) //dont know why but null (needs AI_PASSIVE?) doesnt work
    {
		std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
		return 2;
	}
    
    


    int serverFd, epollFd;

    // Create socket
    serverFd = socket(AF_INET, SOCK_STREAM, 0); //IPPROTO_TCP as last argument also valid?
    if (serverFd == -1) 
    {
        std::cerr << "Failed to create socket." << std::endl;
        freeaddrinfo(result);
        return 1;
    }
    servFdPTR = &serverFd;

    // Bind socket to address and port
    
    
    
    if (bind(serverFd, result->ai_addr, result->ai_addrlen) == -1) 
    {
        std::cerr << "Failed to bind socket." << std::endl;
        freeaddrinfo(result);
        close(serverFd);
        return 1;
    }
    freeaddrinfo(result);

    // Listen for incoming connections --> only saying im "ready" to listen like an open sign on a restaurant
    // Mark socket for accepting incoming connections using accept
    if (listen(serverFd, MAX_CLIENTS) == -1) 
    {
        perror("failed to listen on socket");
        close(serverFd);
        return 1;
    }
    //finished server setup


    struct epoll_event event, epollEvents[MAX_EVENTS];
    // Create epoll instance
    epollFd = epoll_create1(0);
    if (epollFd == -1) {
        std::cerr << "Failed to create epoll instance." << std::endl;
        close(serverFd);
        return 1;
    }
    epollFDPTR = &epollFd;



    // Add server socket to epoll
    event.events = EPOLLIN;
    event.data.fd = serverFd;
    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverFd, &event) == -1) 
    {
        std::cerr << "Failed to add server socket to epoll instance." << std::endl;
        close(serverFd);
        close(epollFd);
        return 1;
    }

    std::cout << "Server started. Listening on port " << static_cast<int>(port) << std::endl;

    while (true) {
        // waiting for event to happen
        int numEvents = epoll_wait(epollFd, epollEvents, MAX_EVENTS, -1);
        if (numEvents == -1) 
        {
            std::cerr << "Failed to wait for events." << std::endl;
            break;
        }

        // Some sockets are ready. Examine readfds --> split to function
        for (int i = 0; i < numEvents; ++i) 
        {
            if 	((epollEvents[i].events & EPOLLIN) == EPOLLIN)
            {
                if (epollEvents[i].data.fd == serverFd) 
                {
                    // Create a client Object here to store the IP-Adress?
                    // Accept new client connection
                    struct sockaddr_storage clientAddress; //storage bigger than normal sockaddr and can hold IPv6
                    socklen_t clientAddressLength = sizeof(sockaddr_storage);
                    int clientFd = accept(serverFd, reinterpret_cast<struct sockaddr*>(&clientAddress), &clientAddressLength);
                    if (clientFd == -1) {
                        std::cerr << "Failed to accept client connection." << std::endl;
                        continue;
                    }
                    std::cout << "Client ip: " << get_ip_str(reinterpret_cast<struct sockaddr*>(&clientAddress)) << std::endl; //

                    // Add client socket to epoll - could event.events be "global", outside of this scope?
                    event.events = EPOLLIN;
                    event.data.fd = clientFd;
                    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &event) == -1) 
                    {
                        std::cerr << "Failed to add client socket to epoll instance." << std::endl;
                        close(clientFd);
                        continue;
                    }

                    // should i try to receive here already just for first connection? pass, nick, user, etc.
                } 
                else // data from an existing connection, receive it
                    handleClient(epollFd, epollEvents[i].data.fd, event);
            }
        }
    }

    close(serverFd);
    close(epollFd);
    return 0;
}





void printIPfrom_getaddinfo()
{
    std::cout << "IP addresses: " << "\n\n";
	/* for(p = result; p != NULL; p= p->ai_next)
    {
		void *addr;
		std::string ipver;

		// get the pointer to the address itself,
		// different fields in IPv4 and IPv6:
		if (p->ai_family == AF_INET)  // IPv4
        {
			struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
			addr = &(ipv4->sin_addr);
			ipver = "IPv4";
		} 
        else // IPv6
        { 
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
			addr = &(ipv6->sin6_addr);
			ipver = "IPv6";
		}

		// convert the IP to a string and print it:
		inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
        std::cout << ipver << ": " << ipstr << std::endl;
	} */
}




