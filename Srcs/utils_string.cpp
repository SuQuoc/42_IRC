
#include "utils_string.hpp"

std::vector<std::string> splitString(const std::string& input, const std::string& delimiter) 
{
    std::vector<std::string> result;

    size_t start = 0;
    size_t pos = input.find(delimiter);

    while (pos != std::string::npos) 
	{
        result.push_back(input.substr(start, pos - start));
        start = pos + delimiter.length();
        pos = input.find(delimiter, start);
    }
    result.push_back(input.substr(start));
    return result;
}


std::vector<std::string> splitIrcCmd(const std::string& input, const std::string& delimiter) 
{
    std::vector<std::string> result;

    size_t start = 0;
    size_t pos = input.find(delimiter);

    while (pos != std::string::npos) 
	{
        result.push_back(input.substr(start, pos - start));
        start = pos + delimiter.length();
        if (input.at(start) == ':')
        {
            result.push_back(input.substr(start));
            break;
        }
        pos = input.find(delimiter, start);
    }
    return result;
}


bool containsForbiddenChars(const std::string& input, const std::string& forbiddenChars) 
{
    // Loop through each character in the input string
    for (std::string::const_iterator it = input.begin(); it != input.end(); it++) 
    {
        // Check if the current character is in the forbidden set
        if (forbiddenChars.find(*it) != std::string::npos) 
        {
            return true;  // Found a forbidden character
        }
    }
    return false;
}

int    splitMsg(std::stringstream& sstream, std::string& str)
{
    char    c;
    int cnt = 0;

    str.clear();
    while (!sstream.eof() && (sstream.peek() =='\n' || sstream.peek() == '\r'))
        sstream.get();
    while (!sstream.eof() && sstream.peek() != '\n' && sstream.peek() != '\r')
    {
        if (!sstream.get(c)) //needs to be checked because it only sets the eof if it trys to read it. Is my theory at least
            break ;
        str += c;
        cnt++;
    }
    if (!sstream.eof())
    {
        str += '\n';
        cnt++;
    }
    return (cnt);
}