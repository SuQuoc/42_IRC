
#include "Irc_error.hpp"



//passing empty string on things that dont requie input?
//if 2 inputs are needed should we?
//takes in Client pointer to send()
void	sendError(IRC_ERR error, Client* sender, const std::string& input)
{
	return ;
	std::stringstream ss;
	ss << error;
	//err;
	std::string err_message;


	//NAME OF THE SERVER
	// err_message = getName(); //Servername, doesnt end with a space
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
		case ERR_UNKNOWNCOMMAND:
			err_message += input + " :Unknown command"; //<command>
			break;
		case ERR_ERRONEUSNICKNAME:
			err_message += input + " :Erroneus nickname"; //<nick>
			break;
		case ERR_NICKNAMEINUSE:
			err_message += input + " :Nickname is already in use"; //<nick>
			break;
		case ERR_USERNOTINCHANNEL:
			err_message += input + " :They aren't on that channel"; //<nick> <channel>
			break;
		case ERR_NOTONCHANNEL:
			err_message += input + " :You're not on that channel"; //<channel>
			break;
		case ERR_USERONCHANNEL:
			err_message += input + " :is already on channel"; //<user> <channel>
			break;
		case ERR_NOTREGISTERED:
			err_message += ":You have not registered";
			break;
		case ERR_NEEDMOREPARAMS:
			err_message += input + " :Not enough parameters"; //<command>
			break;
		case ERR_ALREADYREGISTERED:
			err_message += ":You may not reregister";
			break;
		case ERR_PASSWDMISMATCH:
			err_message += ":Password incorrect"; //!!!!? THIS WILL BE DONE SOMEWHERE ELSE
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
		case ERR_CHANOPRIVSNEEDED:
			err_message += input + " :You're not channel operator"; //<channel>
			break;
		case ERR_USERMODEUNKNOWNFLAG:
			err_message += input + ":Unknown MODE flag";
			break;
		//RPL
		case RPL_WELCOME:
			err_message += "Welcome to the Internet Relay Network" + input; //input = getPrefix() from Client; <nick>!<user>@<host>
			break;

		default:
			std::cout << "CANT HAPPEN DUE TO ENUM" << std::endl;
			//throw ;?
	}
	std::cout << err_message << std::endl;
	send(sender->getFd(), err_message.c_str(), err_message.size(), 0); //--> turn this to a seperat function that sends in a while loop, others outside of switch can also use it 
	//problematic with PASSWORD MISMATCH
}

#include <sstream>

void	sendRPL(IRC_ERR error, Client* sender, const std::string& input)
{
	std::string msg;
	std::string server_name = "AfterLife";
	std::stringstream error_code;

	error_code << error;
	switch (error)
	{
		case RPL_WELCOME:
			msg = ":" + server_name + " 001 " + sender->getNickname() + " :Welcome to the Internet Relay Network, " + input + "\r\n"; //input = getPrefix() from Client; <nick>!<user>@<host>
			break;
		default:
			std::cout << "CANT HAPPEN DUE TO ENUM" << std::endl;
	}
	send(sender->getFd(), msg.c_str(), msg.size(), 0);
}