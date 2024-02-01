
#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <sstream>
#include <vector>
#include <string>

bool		containsForbiddenChars(const std::string& input, const std::string& forbiddenChars);
int			splitMsg(std::stringstream& sstream, std::string& str);
std::string	extractWord(std::stringstream& sstream);
int 		protectedSend(int fd, std::string msg);

