

# include "irc.hpp"

const int MAX_EVENTS = 10;
const int MAX_CLIENTS = 10;

void sendToClient(int clientFd, const std::string& message)
{
    if (send(clientFd, message.c_str(), message.size(), 0) == -1)
        perror("send");
}

void cleanClientFd(int epollFd, int clientFd, epoll_event& event)
{
	if (epoll_ctl (epollFd, EPOLL_CTL_DEL, clientFd, &event) == -1) // do i even need this event if i delete a client from epoll instance
        perror ("epoll_ctl");
    if (close (clientFd) == -1)
        perror ("close");
}

void ClientConnectionAttempt(std::string pass_msg)
{
    std::string _serverPW = "test";
    std::string buf;

    std::istringstream iss(pass_msg);

    std::getline(iss, buf, ' ');
    if (iss.eof())
        return;
    else if (iss.fail()) 
    {
        std::cerr << "Error reading input." << std::endl;
        return ;
    }
    else if (buf == "PASS")
    {
        std::getline(iss, buf, '\r');
        if (buf == _serverPW)
            initClient(); //setting nickname 
        else
            cleanClientFd();
    }
    else 
        return ;
}


// Function to handle client connections
void handleClient(int epollFd, int clientFd, epoll_event& event) 
{
	char buf[MAX_MSG_LEN];
    ssize_t numbytes = recv(clientFd, &buf, MAX_MSG_LEN, 0); // should flag set to non block? does epoll make it unnecessary? MSG_DONTWAIT

    std::string message(buf); //max message len
    if (numbytes == -1)
        perror ("recv error");
    else if (numbytes == 0) // connection closed by client with QUIT message, not sure if this means that he's completely out from the server?
    {
        std::cout << "Socket " << clientFd << " closed by client" << std::endl;
        // delete fd from epoll
        cleanClientFd(epollFd, clientFd, event);
    }
	// else if (validateClient()) //check passw, nickname, username
    // {
        // cleanClientFd(epollFd, clientFd, event);
	// }
	else 
    {
        //handleClientMessage();
        std::cout << "Client[" << clientFd << "]: " << message << std::endl;
        sendToClient(clientFd, "Message received!\n");
    }
    std::cout << "Client[test]: " << message << std::endl;
}


uint16_t convertPort(const std::string& str)
{
    uint16_t port;
    std::stringstream ss(str);
    
    try 
    {
        // Extract the unsigned short from the stringstream
        ss >> port;

        if (ss.fail()) {
            // Conversion failed
            std::cerr << "Conversion failed. The string is not a valid unsigned short.\n";
        } else {
            // Conversion succeeded
            std::cout << "Converted unsigned short: " << port << std::endl;
        }

        if (!ss.eof()) 
        {
            std::cerr << "Invalid characters in input.\n";
            return 1;
        }

        std::cout << "Parsed port number: " << port << std::endl;
    } 
    catch (const std::out_of_range& e) 
    {
        std::cerr << "Out of range error: " << e.what() << std::endl;
        return 1;
    }
    catch (const std::invalid_argument& e)
    {
        std::cerr << "Invalid argument error: " << e.what() << std::endl;
        return 1;
    }
    return port;
}


int main(int argc, char **argv) 
{
    if (argc != 3) {
	    std::cerr << "Usage: ./ircserv \"port (6667)\" \"password\"\n";
	    return 1;
	}

    const std::string _port = argv[1];
	const std::string _pw = argv[2];



    struct addrinfo hints, *result, *p;
	int status;
	char ipstr[INET6_ADDRSTRLEN];
	
	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
	hints.ai_socktype = SOCK_STREAM;
	if ((status = getaddrinfo("localhost", _port.c_str(), &hints, &result)) != 0) 
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


    // Bind socket to address and port
    
    int port = convertPort(_port);
    if (port == -1)
    {
        perror("portconvert");
    }
    
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
                    // Accept new client connection
                    struct sockaddr_storage clientAddress; //storage bigger than normal sockaddr and can hold IPv6
                    socklen_t clientAddressLength = sizeof(sockaddr_storage);
                    int clientFd = accept(serverFd, reinterpret_cast<struct sockaddr*>(&clientAddress), &clientAddressLength);
                    if (clientFd == -1) {
                        std::cerr << "Failed to accept client connection." << std::endl;
                        continue;
                    }

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