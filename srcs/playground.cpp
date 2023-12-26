#include <iostream>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <vector>
#include <errno.h> 
#include <stdio.h> 

#include <cstring>
#include <iostream>
#include <sstream>
#include <string>


const int MAX_EVENTS = 10;
const int MAX_CLIENTS = 10;
const int PORT = 8080;

// Function to handle client connections in a separate thread
void handleClient(int epollFd, int clientFd, epoll_event& event) 
{
    char recv_message[1000]; //max message len
    std::memset(&recv_message, '\0', sizeof (recv_message));
    ssize_t numbytes = recv(clientFd, &recv_message, sizeof(recv_message), 0); // should flag set to non block?

    if (numbytes == -1)
        perror ("recv");
    else if (numbytes == 0) // connection closed by client
    {
        std::cout << "Socket " << clientFd << " closed by client" << std::endl;
        // delete fd from epoll
        if (epoll_ctl (epollFd, EPOLL_CTL_DEL, clientFd, &event) == -1) // do i even need this event if i delete a client from epoll instance
            perror ("epoll_ctl");
        if (close (clientFd) == -1)
            perror ("close");
    }
    else 
    {
        std::cout << "Client[" << clientFd << "]: " << recv_message << std::endl;
    }
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

        if (!ss.str().empty()) 
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
    if (argc != 2) {
	    std::cerr << "usage: showip hostname\n";
	    return 1;
	}

    const std::string _port = argv[1];

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
    
    std::cout << "IP addresses for " << argv[1] << "\n\n";
	for(p = result; p != NULL; p= p->ai_next)
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
	}


    int serverFd, epollFd;
    

    // Create socket
    serverFd = socket(AF_INET, SOCK_STREAM, 0); //IPPROTO_TCP as last argument also valid?
    if (serverFd == -1) 
    {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }


    std::string portSTR = argv[1];
    // Bind socket to address and port
    
    uint16_t port = convertPort(portSTR);
    if (bind(serverFd, result->ai_addr, result->ai_addrlen) == -1) 
    {
        std::cerr << "Failed to bind socket." << std::endl;
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

    std::cout << "Server started. Listening on port " << PORT << std::endl;

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

                    // would be cool print IP address of the new client
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
