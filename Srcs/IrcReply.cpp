
#include "../Includes/IrcReply.hpp"


IrcReply::IrcReply(const std::string& server_name): _server_name(server_name){}
IrcReply::~IrcReply(){}


//passing empty string on things that dont requie input?
//takes in Client pointer to send()
int	IrcReply::sendError(IRC_ERR error, Client* sender, const std::string& input) const
{
	std::string err_message;
	std::stringstream error_code;

	error_code << error;
	if(sender->getNickname().empty() == true)
		err_message += ":" + _server_name + " " + error_code.str() + " ";
	else
		err_message += ":" + _server_name + " " + error_code.str() + " " + sender->getNickname() + " ";
	switch (error)
	{
		case ERR_NOSUCHNICK:
			err_message += input + " :No such nick/channel"; //<nickname>
			break;
		case ERR_NOSUCHCHANNEL:
			err_message += input + " :No such channel"; //<channel name>
			break;
		case ERR_CANNOTSENDTOCHAN:
			err_message += input + " :Cannot send to channel"; //<channel name>
			break;
		case ERR_TOOMANYCHANNELS:
			err_message += input + " :You have joined too many channels"; //<channel name>
			break;
		case ERR_TOOMANYTARGETS:
			err_message += input + " :Duplicate recipients. No message delivered"; //<target> ????? very weird stil when this is triggered --> protocoll
			break;
		case ERR_NORECIPIENT:
			err_message += ":No recipient given (" + input + ")"; //just privmsg
			break;
		case ERR_NOTEXTTOSEND:
			err_message += ":No text to send";
			break;
		case ERR_UNKNOWNCOMMAND:
			err_message += input + " :Unknown command"; //<command>
			break;
		case ERR_NONICKNAMEGIVEN:
			err_message += ":No nickname given";
			break;
		case ERR_ERRONEUSNICKNAME:
			err_message += input + " :Erroneus nickname"; //<nick>
			break;
		case ERR_NICKNAMEINUSE:
			err_message += input + " :Nickname is already in use"; //<nick>
			break;
		case ERR_USERNOTINCHANNEL: //for modes
			err_message += input + " :They aren't on that channel"; //<nick> <channel>
			break;
		case ERR_NOTONCHANNEL:
			err_message += input + " :You're not on that channel"; //<channel>
			break;
		case ERR_USERONCHANNEL:
			err_message += input + " :is already on channel"; //<user> <channel>
			break;
		case ERR_NOTREGISTERED:
			err_message = ":" + _server_name + " " + error_code.str() + " :You have not registered";
			break;
		case ERR_NEEDMOREPARAMS:
			err_message += input + " :Not enough parameters"; //<command>
			break;
		case ERR_ALREADYREGISTERED:
			err_message += ":You may not reregister";
			break;
		case ERR_PASSWDMISMATCH:
			err_message = ":" + _server_name + " " + error_code.str() + " :Password incorrect"; //!!!!? THIS WILL BE DONE SOMEWHERE ELSE
			break;
		case ERR_KEYSET:
			err_message += input + " :Channel key already set"; //<channel>
			break;
		case ERR_CHANNELISFULL:
			err_message += input + " :Cannot join channel (+l)"; //<channel>
			break;
		case ERR_UNKNOWNMODE:
			err_message += input + " :is unknown mode char to me"; //<char>
			break;
		case ERR_INVITEONLYCHAN:
			err_message += input + " :Cannot join channel (+i)"; //<channel>
			break;
		case ERR_BADCHANNELKEY:
			err_message += input + " :Cannot join channel (+k)"; //input?
			break;
		case ERR_CHANOPRIVSNEEDED:
			err_message += input + " :You're not channel operator"; //<channel>
			break;
		case ERR_USERMODEUNKNOWNFLAG:
			err_message += input + ":Unknown MODE flag"; //input?
			break;
		case ERR_NOOPERHOST:
			err_message += ":No O-lines for your host";
			break;
		case ERR_NOPRIVILEGES:
			err_message += ":Permission Denied- You're not an IRC operator";
			break;
		default:
			std::cout << "CANT HAPPEN DUE TO ENUM" << std::endl;
	}
	protectedSend(sender->getFd(), err_message);
	return (error);
}

void	IrcReply::sendRPL(IRC_ERR error, Client* sender, const std::string& input) const
{
	std::string msg;
	std::stringstream error_code;

	error_code << error;
	msg = ":" + _server_name + " " + error_code.str() + " " + sender->getNickname() + " ";
	switch (error)
	{
		case RPL_CHANNELMODEIS:
			msg += input;
			break;
		case TOPIC_SET:
			msg = ":" + sender->getPrefix() + " TOPIC " + input;
			break;
		case RPL_JOIN:
			msg = ":" + sender->getPrefix() + " JOIN " + input + " * :" + sender->getUsername();
			break;
		case RPL_WELCOME:
			msg = ":" + _server_name + " 001 " + sender->getNickname() + " :Welcome to the Internet Relay Network, " + input; //input = getPrefix() from Client; <nick>!<user>@<host>
			break;
		case RPL_ISUPPORT:
			msg = ":" + _server_name + " 005 " + sender->getNickname() + " NICKLEN=9 CHANTYPES=#& CHANMODES=oiklt :are supported by this server";
			break;
		case RPL_YOUREOPER:
			msg += ":You are now an IRC operator";
			break;
		case RPL_TOPIC:
			msg = input; //"<channel> :<topic>"
			break;
		case RPL_NOTOPIC:
			msg = input + " :No topic is set"; //channel
			break;
		case RPL_INVITING:
			msg += input;
			break;
		default:
			std::cout << "CANT HAPPEN DUE TO ENUM" << std::endl;
	}
	protectedSend(sender->getFd(), msg);
}

