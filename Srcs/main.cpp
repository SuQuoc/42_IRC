#include "../Includes/Irc.hpp"
#include <csignal>

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

void signalHandler(int signum) {
	if(signum == SIGINT)
    	std::cout << " Signal SIGINT(" << signum << ") received." << std::endl;
}

void signalHandler()
{
	struct sigaction sigact;
    sigact.sa_handler = signalHandler; // Set signal handler function
    sigemptyset(&sigact.sa_mask); // Clear the signal mask
	sigact.sa_flags = 0;

    sigaction(SIGINT, &sigact, NULL);
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
	signalHandler();
	Irc	server("AfterLife", argv[2]);
	server.setOperatorHost(OPER_IP);
	server.setOperatorPW(OPER_PW);
	if (server.createTcpSocket(stoi_(argv[1])) == -1)
		return (1);
	server.creatEpoll();
	try {
		server.pollLoop();
	} catch (std::exception& e) {
		std::cerr << "Error: thrown: " << e.what() << std::strerror(errno) << std::endl;
	}
}
