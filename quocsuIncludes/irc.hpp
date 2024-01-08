# pragma once

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

# include <csignal>


# define MAX_MSG_LEN 512

// clienthandler.cpp
void handleClient(int epollFd, int clientFd, epoll_event& event);

// port.cpp
uint16_t convertPort(const std::string& str);

//string_utils.cpp
std::vector<std::string> splitString(const std::string& input, const std::string& delimiter);

//utils_string.cpp
std::vector<std::string> splitString(const std::string& input, const std::string& delimiter);

