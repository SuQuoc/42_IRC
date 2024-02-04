
#pragma once

#include "../Includes/Client.hpp"
#include <sstream>

class Client;
class Channel;

enum IRC_ERR
{
    ERR_NOSUCHNICK = 401,
    ERR_NOSUCHCHANNEL = 403,
    ERR_CANNOTSENDTOCHAN = 404,
    ERR_TOOMANYCHANNELS = 405,
    ERR_TOOMANYTARGETS = 407,
    ERR_NORECIPIENT = 411,
    ERR_NOTEXTTOSEND = 412,
    ERR_UNKNOWNCOMMAND = 421,
    ERR_NONICKNAMEGIVEN = 431,
    ERR_ERRONEUSNICKNAME = 432,
    ERR_NICKNAMEINUSE = 433,
    ERR_USERNOTINCHANNEL = 441,
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
    ERR_BADCHANNELKEY = 475,
    ERR_NOPRIVILEGES = 481,
    ERR_CHANOPRIVSNEEDED = 482, 
    ERR_USERMODEUNKNOWNFLAG = 501,
    ERR_USERSDONTMATCH = 502,
    ERR_NOOPERHOST = 491,

    RPL_WELCOME = 001,
    RPL_ISUPPORT = 005,
    RPL_YOUREOPER = 381,
    RPL_NOTOPIC = 331,
    RPL_TOPIC = 332,
    RPL_NAMREPLY = 353,
    RPL_CHANNELMODEIS = 324,
    RPL_UMODEIS = 221,
    RPL_INVITING = 341,

    //own codes!
    RPL_JOIN = 1000,
    TOPIC_SET = 1001,
    RPL_INVITED = 1002,
    MODE_SET_PLUS = 1003,
    MODE_SET_MINUS = 1004
};

class IrcReply
{
private:
    const std::string& _server_name;

    IrcReply(const IrcReply&);
    IrcReply& operator=(const IrcReply&);
public:
    IrcReply(const std::string& server_name);
    ~IrcReply();
    
    int     sendError(IRC_ERR error, Client* sender, const std::string& input) const;
    void	sendRPL(IRC_ERR error, Client* sender, const std::string& input) const;
};
