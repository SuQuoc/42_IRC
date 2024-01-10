
#include <string>
#include <vector>
#include <iterator>

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

void command_switch(const std::string& message) //request better name? for us to discern
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










void	PASS(Client *sender, std::stringstream &sstream);
{
    std::string password;
    std::getline(sstream, password);

    if (password.empty())
        sendError(ERR_);
    else if (sender->isAuthenticated())
        sendError(ERR_);
    else if (password == _serverPW) //gehört noch in Abstract Server
        sender->setToAuthenticated(); //bissi blöder name
    else
        sendError(ERR_);
        // delete the user von server already?
}

void	NICK(Client *sender, std::stringstream &sstream);
{
    std::string nickname;
    std::getline(sstream, nickname); //what if nick has space is it being ignored are is space not allowed
    
    if (nickanme.empty())
        sendError(ERR_);
    else if (sender.isAuthenticated() == false)
        sendError(ERR_); //or return 
    else if (!isNormed(nickname)) // are spaces allowed? getline currently loops until \n
        sendError(ERR_); //or return 

    it = _fd_map.find(nickname) //key may not be used cuz it creates an entry-->actually good for us?
    if (it == map.end()) //no one has the nickname
        sender.setNickname();
}

//USER username hostname servername :realname
//USER JohnDoe localhost irc.example.com :John Doe
void	USER(Client *sender, std::stringstream &sstream)
{
    std::vector<std::string> info(4);
    //std::string username;
    //std::string hostname;
    //std::string servername;
    //std::string realname;


    for (std::vector<std::string>::it = it.begin(); it != info.end(); it++)
    {
        std::getline(sstream, *it);
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


