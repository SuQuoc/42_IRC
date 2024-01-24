#include "../../Includes/Irc.hpp"

static int	stoi_(const std::string str)
{
	std::stringstream	stream(str);
	int					i;

	stream >> i;
	return (i);
}


static bool isValidPort(const std::string& port)
{
    // Check if the portStr is a positive integer
	for(std::string::const_iterator it = port.begin(); it != port.end(); ++it) 
	{
        if (!std::isdigit(*it))
		{
			std::cerr << "Only numeric values for the port allowed" << std::endl;
            return false;
		}
    }

    int portnumber = stoi_(port);
	
	if (portnumber < 1024 || 49151 < portnumber)
	{
		std::cerr << "Error: Please choose a port reserved for server applications 1024 - 49151" << std::endl;
		return false;
	}
    return true;
}

static bool isValidPassword(const std::string& password) 
{
    // Check if the password contains any whitespaces
	int len = password.size();
    if (len < 8 || len > 30)
	{
		std::cerr << "Error: password must be at least 8 and maximum 30 chars long" << std::endl;
		return false;
	}
	
	for(std::string::const_iterator it = password.begin(); it != password.end(); ++it) 
	{
        if (std::isspace(*it)) 
		{
			std::cerr << "Error: No whitespaces allowed in password" << std::endl;
            return false;
        }
    }
    return true;
}

int main(const int argc, const char *argv[])
{
	if (argc != 3)
	{
		std::cerr << "Error: the server needs an IPv4-ADDRESS (e.g. 172.0.0.10) as first argument and a PORT (e.g. 8080) as second argument" << std::endl;
		return (0);
	}
	if (!isValidPort(argv[1]) || !isValidPassword(argv[2]))
		return (0);

	Irc	server(argv[2]);
	//check port and ip-adress!!! //not ip-adress but password!!!!!!!!!!!!!

	Client c1(10);
    Channel ch1(&c1, "Test");
    c1.joinChannel(&ch1);
	c1.leaveChannel(&ch1);
}