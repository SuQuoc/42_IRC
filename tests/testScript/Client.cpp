#include "Client.hpp"

Client::Client(const int &client_fd) : _client_fd(client_fd)
{

}
Client::Client(const int &client_fd, const std::string &user_name, const std::string &nick_name) : _user_name(user_name), _nick_name(nick_name), _client_fd(client_fd)
{

}
Client::Client(const Client &C) : _user_name(C._user_name), _nick_name(C._nick_name), _client_fd(C._client_fd)
{

}
Client Client::operator=(const Client &C)
{
    _user_name = C._user_name;
	_nick_name = C._nick_name; // should we copy everything?
    return *this;
}
Client::~Client() {}

// need to add client actions send msg and so on
void Client::joinChannel( Channel *new_channel )
{
    if(new_channel == NULL)
        return ;
    //check if invte only
    _channels.push_back(new_channel);
}

//set and get
const std::string &Client::getUserName() const { return _user_name; }

const std::string &Client::getNickName() const { return _nick_name; }

const int &Client::getClientFd() const { return _client_fd; }

const std::vector<Channel *> &Client::getChannels() const { return _channels; }

void Client::setUserName(const std::string user_name) { _user_name = user_name; }

void Client::setNickName(const std::string nick_name) { _nick_name = nick_name; }
