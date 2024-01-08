#include "../Includes/IrcServer.hpp"

static int	stoi_(const std::string str)
{
	std::stringstream	stream(str);
	int					i;

	stream >> i;
	return (i);
}

int main(const int argc, const char *argv[])
{
	IrcServer	server;

	if (argc != 3)
	{
		std::cerr << "Error: the server needs an IP4-ADRESS (e.g. 172.0.0.10) as first argument and a PORT (e.g. 8080) as second argument" << std::endl;
		return (0);
	}
	//check port and ip-adress!!! //not ip-adress but password!!!!!!!!!!!!!
	server.createTcpSocket(argv[1], stoi_(argv[2]));
	server.createEpoll();
	server.epollLoop();
}
