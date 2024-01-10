

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

void IRC_Commands(const std::string& message) //request better name? for us to discern
{

    sstream(string)
    std::string cmd  = getline();


    if (cmd == "PASS") PASS()
    else if (cmd == "NICK") NICK()
    else if (cmd == "USER") USER()
    else if (cmd =="PRIVMSG") PRIVMSG()
    else if (cmd =="JOIN") JOIN()
    else if (cmd =="PART") PART()
    else if (cmd =="QUIT") QUIT()
    else if (cmd =="KICK") KICK()
    else if (cmd =="INVITE") INVITE()
    else if (cmd =="MODE") MODE()
    else if (cmd =="TOPIC") TOPIC()
    else sendError(ERR_UNKNOWNCOMMAND) //enum, dummer name
}










void PASS(client, stream)
{
    std::string password = getline(stream);


    if (password == _serverPW) //gehört noch in Abstract Server
        client.setToAuthenticated(); //bissi blöder name
    else
        sendError(ERR_);
        // delete the user von server already?



}

void NICK(client, stream)
{
    std::string nickname = getline(stream);
    
    if (client.isAuthenticated() == false)
         sendError(ERR_); //or return 
    if (!isNormed(nickname))
        sendError(ERR_); //or return 

    find user by nickname in map
    if (it == map.end()) //no one has the nickname
        

}


void USER(client, stream)
{
    std::


}





//PRIVMSG #channel
void PRIVMSG(const Client* sender, const Client* receiver)
void PRIVMSG(const Client* sender, const Channel* name)





void KICK(const Client* sender, )



void MODE()




void delUserFromChannel()
{
    must check if user was the last one 
}

void PART(const Client* sender, )
{
    look for channel to leave if not found --> Error

    if found
        channel[i]->delUsersFromChannel()

    
}

void QUIT()
{

    for ()
        server._users_fd[fd]->channel[i]->delUserFromChannel()
    

    _users_name.erase(_users_fd[fd].nickname)
    delete _Users_fd[fd]
    _users_fd.erase(fd) 


}


