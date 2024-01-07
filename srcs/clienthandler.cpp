
# include "irc.hpp"
# include "Client.hpp"

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

void registerClient(const std::string& registration)
{
    splitString(registration, "\r\n");
}

/* void assesCmd(std::string pass_msg) //can this be checked only once somehow? or is it normal to check every incoming send()
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
       registerClient();
    }
    else
        exec
    return ;
} */


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
	else 
    {
        
        //exeClientRequest(message, server.getClient(clientFD)); 
        //write getter to get client object by their fd
        //is it better to pass an object thats already in the server class? or should i pass the fd (also in server class) and look for the client later
        std::cout << "Client[" << clientFd << "] with ip[]: " << message << std::endl;
        // sendToClient(clientFd, "Message received!\n");
        sendToClient(clientFd, "10.13.4.6 001 quoc :Welcome to the IRC Network, username!\r\n");
    }
    // std::cout << "Client[test]: " << message << std::endl;
}

bool isRegistered(Client& client)
{
    if (client.getNickname().empty())
        return false;
    return true;
}
bool isRegAttempt(const std::string& command)
{
    // just check if it starts with PASS i guess registerClient will handle the rest like incorrect params or pw
    if (command.substr(0, 5) == "PASS ")
        return true;
    return false;
}

void registerClient()
{
    std::cout << "Succesfully registered";
}

/* void exeClientRequest(const std::string& command, Client& client)
{
    bool registered = isRegistered(client);
    bool regAttempt = isRegAttempt();

    if (!registered && !regAttempt)
        std::cout << "not registered";
    else if (!registered && regAttempt)
        registerClient(); //"PASS"
    else if (registered && regAttempt)
        std::cout << "already registered";
    else 
    {
        std::cout << "exec other cmds";
        // find CMD_STR in map and execute cmd
        // every IRC CMD takes a REF to User Object
    }
} */