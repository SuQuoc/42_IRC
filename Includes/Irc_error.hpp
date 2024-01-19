
#pragma once

#include "../Includes/Client.hpp"

class Client;
class Channel;

enum IRC_ERR
{
    ERR_NOSUCHNICK = 401,
    ERR_NOSUCHCHANNEL = 403,
    ERR_CANNOTSENDTOCHAN = 404, //? very weird, not for the case i expected
    ERR_TOOMANYCHANNELS = 405, //our limit 10?
    ERR_TOOMANYTARGETS = 407, //?dont want to implement that
    ERR_NORECIPIENT = 411,
    ERR_NOTEXTTOSEND = 412, 
    ERR_UNKNOWNCOMMAND = 421,
    ERR_ERRONEUSNICKNAME = 432,
    ERR_NICKNAMEINUSE = 433,
    ERR_USERNOTINCHANNEL = 441, //?
    ERR_NOTONCHANNEL = 442,
    ERR_USERONCHANNEL = 443,
    ERR_NOTREGISTERED = 451,
    ERR_NEEDMOREPARAMS = 461,
    ERR_ALREADYREGISTERED = 462,
    ERR_PASSWDMISMATCH = 464,
    ERR_KEYSET = 467,
    ERR_CHANNELISFULL = 471,
    ERR_UNKNOWNMODE = 472,
    ERR_INVITEONLYCHAN = 473,
    ERR_NOPRIVILEGES = 481, //? IRC operator
    ERR_CHANOPRIVSNEEDED = 482, 
    ERR_USERMODEUNKNOWNFLAG = 501,
    ERR_USERSDONTMATCH = 502, //?

    RPL_WELCOME = 001,
    RPL_YOUREOPER = 381,
    RPL_NOTOPIC = 331,
    RPL_TOPIC = 332,
    RPL_NAMREPLY = 353,
    RPL_CHANNELMODEIS = 324, //?
    RPL_UMODEIS = 221,//?
    RPL_INVITING = 341
};

void	sendError(IRC_ERR error, Client* sender, const std::string& input);
void	sendRPL(IRC_ERR error, Client* sender, const std::string& input);
