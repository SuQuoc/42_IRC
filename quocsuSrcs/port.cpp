
# include "irc.hpp"

uint16_t convertPort(const std::string& str)
{
    uint16_t port;
    std::stringstream ss(str);
    
	// Extract the unsigned short from the stringstream
	ss >> port;

	if (ss.fail()) 
	{
		std::cerr << "Invalid port" << std::endl;
		return 1; //exit 
	}
	else if (!ss.eof()) 
	{
		std::cerr << "Invalid characters AT END OF port.\n";
		return 1;
	}
	std::cout << "Requested port: " << port << std::endl;
    return port;
}
