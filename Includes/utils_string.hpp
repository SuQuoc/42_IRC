
#pragma once

#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>

#include <iostream>
#include <cstdlib>
#include <cstring> //only strerror()
#include <sstream>
#include <vector>
#include <string>


bool		containsForbiddenChars(const std::string& input, const std::string& forbiddenChars);
int			splitMsg(std::stringstream& sstream, std::string& str);
std::string	extractWord(std::stringstream& sstream);
void 		protectedSend(int fd, std::string msg);

