
#include "irc.hpp"

void sendErrorToClient(IRC_ERR error)
{
	switch (error)
	{
		case ERR_NOSUCHCHANNEL:
			break;
		case ERR_ERRONEUSNICKNAME:
			break;
		case ERR_NICKNAMEINUSE:
			break;
		default:
			throw smth;
	}
}
