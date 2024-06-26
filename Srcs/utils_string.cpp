#include "../Includes/utils_string.hpp"
#include "AServer.hpp"

bool    containsForbiddenChars(const std::string& input, const std::string& forbiddenChars) 
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

int splitMsg(std::stringstream& sstream, std::string& str)
{
    char    c;
    int cnt = 0;

    str.clear();
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
    while (!sstream.eof() && (sstream.peek() == '\n' || sstream.peek() == '\r'))
        sstream.get();
    return (cnt);
}

std::string extractWord(std::stringstream& sstream)
{
	std::string	word;

	sstream >> std::ws;
	if (sstream.peek() == ':')
	{
		sstream.get();
		std::getline(sstream, word);
	}
	else
		std::getline(sstream, word, ' ');
	return (word);
}

//joins \\r\\n to end of msg		\it's double '\' so it shows in description
