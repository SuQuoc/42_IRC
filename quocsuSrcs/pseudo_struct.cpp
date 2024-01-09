

void IRC_Commands(std::string )
{
    switch()
    {
        case PRIVMSG:
            if ()
                PRIVMSG();
            else 
                PRIVMSG(channel);
        case JOIN:
            JOIN();
    }



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


