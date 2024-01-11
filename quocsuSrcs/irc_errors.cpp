
#include "irc.hpp"

void sendErrorToClient(IRC_ERR error, Client* sender)
{
	std::string error_message;
	error_message = sender.getPrefix() 
	switch (error)
	{
		case ERR_NOSUCHCHANNEL:
			error_message += "LOOK AT IRC PROTOCOLL";
			break;
		case ERR_ERRONEUSNICKNAME:
			error_message += "LOOK AT IRC PROTOCOLL";
			break;
		case ERR_NICKNAMEINUSE:
			break;
		default:
			throw smth;
	}
	sender.sendMsg();
}
