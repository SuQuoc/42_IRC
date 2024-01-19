
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

//can the PREFIX we get from the client be always ignored??
split_irc_message()
{
    std::string trailing;
    std::string prefix;

    //params contains also optional parameters and params2 is only used if cmd supports 2 parameter listing
    //if it doest support listing then just loop through this
    //each cmd loops through params, some could only take n params from the vector and ignore the rest
    std::vector<std::string> params; 
    std::vector<std::string> params2; //arguments? better name?

    tmp = getline();
    if (tmp == prefix())
        prefix = tmp
        cmd = getline();
    else
        cmd = tmp;
    para = getline();
    if (supportsMessage(cmd)) //USER, PRIVMSG, PART, QUIT, KICK --> maybe we dont have to check if it supports and do it everytime
    {
        msg = find first SPACE-COLON " :" and cut everything after it out from string;
    }
    params = splitAtSpace(para); //now params could be filled with #c1,#c2,#c3 and its getting turned into 3 diff strings
    if (supportsListing(cmd)) //or we check if the strings contain a ,?
        split params at ',' fill params2 with the splited params;
        params contains the spliited strings from the first string in params;

    //channel name can have :
}











//USER username hostname servername :realname
//USER JohnDoe localhost irc.example.com :John Doe






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


