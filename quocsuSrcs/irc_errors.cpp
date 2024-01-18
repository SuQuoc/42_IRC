
#include "irc.hpp"

enum IRC_ERR
{
    ERR_NOSUCHCHANNEL = 403, //? can this happen with our Reference client, when does it occur
    ERR_CANNOTSENDTOCHAN = 404, //?
    ERR_UNKNOWNCOMMAND = 421,
    ERR_ERRONEUSNICKNAME = 432,
    ERR_NICKNAMEINUSE = 433,
    ERR_USERNOTINCHANNEL = 441, //?
    ERR_NOTONCHANNEL = 442, //?
    ERR_USERONCHANNEL = 443,
    ERR_NOTREGISTERED = 451,
    ERR_NEEDMOREPARAMS = 461,
    ERR_ALREADYREGISTERED = 462,
    ERR_PASSWDMISMATCH = 464,
    ERR_UNKNOWNMODE = 472, //?
    ERR_CHANOPRIVSNEEDED = 482, 
    ERR_USERMODEUNKNOWNFLAG = 501, //?
    ERR_USERSDONTMATCH = 502 //?
};


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
