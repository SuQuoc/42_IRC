#include "Client.hpp"

Client::Client(const int &client_fd) : _client_fd(client_fd)
{

}
Client::Client(const int &client_fd, const std::string &user_name, const std::string &nick_name) : _client_fd(client_fd), _user_name(user_name), _nick_name(nick_name)
{

}
Client::Client(const Client &C) : _user_name(C._user_name), _nick_name(C._nick_name), _client_fd(C._client_fd)
{

}
Client Client::operator=(const Client &C)
{
    _user_name = C._user_name;
	_nick_name = C._nick_name; // should we copy everything?
}
Client::~Client() {}

// need to add client actions send msg and so on

//set and get
const std::string &Client::getUserName() { return _user_name; }

const std::string &Client::getNickName() { return _nick_name; }

const int &Client::getClientFd() { return _client_fd; }

void Client::setUserName(const std::string user_name) { _user_name = user_name; }

void Client::setNickName(const std::string nick_name) { _nick_name = nick_name; }
