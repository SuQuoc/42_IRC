

#include "Client.hpp"


Client& Server::getClient(int fd)
{
	std::vector<Client*> _clients;

	for (std::vector<Client*>::iterator it = _clients.begin(); it != _clients.end(); iter++)
	{
		if (fd == it->_connecSocket)
			return (*it)
	}
	return (NULL); //if this doesnt work try catch maybe?
}