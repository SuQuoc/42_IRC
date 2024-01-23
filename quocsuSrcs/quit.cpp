
//does QUIT only send the message to all channels the client was in??
void	Irc::QUIT(Client *sender, std::stringstream &sstream)
{
	std::string	channel_name;
	std::string	msg = extractWord(sstream);
	if (msg.empty())
		msg = "disconnected";
	
	// get const channel-vector reference from client-map(?); can i t be const? i delete it!!
	std::vector<Channel *> channels = sender->getAllChannels(); //returns a const vector which doesnt make sense	
	
	for (std::vector<Channel *>::iterator it = channels.begin(); it != channels.end(); it++)
	{
		Channel *channel = (*it);
		// sender->leaveChannel(channel); //unecessary ?? he will leave entire server
		channel_name = channel->getName();
		// if (channel->rmClient(sender, msg) == CHANNEL_DEAD) //delete channel when empty(), Server sending msg or rmClient with an add msg para???
			// rmChannel(channel_name);		
	}
	rmClient(sender);
}