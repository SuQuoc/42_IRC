
#include "irc.hpp"


USER(std::string& )
{
	if (param < 4)
		ERR_NEEDMOREPARAMS
	if (usernameSET)
		ERR_ALREADYREGISTERED
}