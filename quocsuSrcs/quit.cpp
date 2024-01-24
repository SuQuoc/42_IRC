
//does QUIT only send the message to all channels the client was in??
void	Irc::QUIT(Client *sender, std::stringstream &sstream)
{
	std::string	channel_name;
	std::string	msg = extractWord(sstream);
	if (msg.empty())
		msg = "disconnected";
	
	std::vector<Channel *> channels = sender->getAllChannels();
	for (std::vector<Channel *>::iterator it = channels.begin(); it != channels.end(); it++)
	{
		Channel *channel = (*it);
		// sender->leaveChannel(channel); //unecessary ?? he will leave entire server
		channel_name = channel->getName();
		//delete channel when empty(), Server sending msg or rmClient with an add msg para???
		if (channel->rmClient(sender, msg) == -1) //exchange -1 with CHANNEL_DEAD
			rmChannel(channel_name);		
	}
	rmClient(sender);
}


