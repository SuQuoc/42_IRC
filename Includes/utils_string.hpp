
#pragma once

#include <string>
#include <vector>
#include <sstream>

bool	containsForbiddenChars(const std::string& input, const std::string& forbiddenChars);
int		splitMsg(std::stringstream& sstream, std::string& str);
std::string	extractWord(std::stringstream& sstream);

