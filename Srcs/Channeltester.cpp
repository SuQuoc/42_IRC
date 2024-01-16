#include "../Includes/Channel.hpp"

Client *addUser(const std::string &name)
{
    Client *niki = new Client(3);

    niki->setUser("niki", "niki", "niki", "niki");
    niki->setNickname("niki");
    return niki;
}

void test_IsOperator(Channel &after_life, Client *niki)
{
    after_life.isOperator(NULL);
    if(after_life.isOperator(niki) == true)
        std::cout << niki->getUsername() << " is in channel " << after_life.getName() << std::endl;
    else if(after_life.isOperator(niki) == false)
        std::cout << niki->getUsername() << " is not in channel " << after_life.getName() << std::endl;
}

void    test_addClient(Channel &after_life, Client *beni)
{
    after_life.addClient(beni, false);
    std::cout << "addClient: ";
    if(after_life.isInChannel(beni) == true)
        std::cout << "ok";
    else
        std::cout << "ko";
    std::cout << std::endl;
}

void    test_rmClient(Channel &after_life, Client *beni)
{
    test_addClient(after_life, beni);
    after_life.rmClient(beni);
    std::cout << "rmClient: ";
    if(after_life.isInChannel(beni) == false)
        std::cout << "ok";
    else
        std::cout << "ko";
    std::cout << std::endl;
    after_life.rmClient(beni);
}

void 	test_isInChannel(Channel &after_life, Client *niki, Client *beni)
{
    std::cout << "isInChannel: ";
    if(after_life.isInChannel(niki) == true)
        std::cout << "ok ";
    else
        std::cout << "ko ";
    if(after_life.isInChannel(beni) == true)
        std::cout << "ko ";
    else
        std::cout << "ok ";
    std::cout << std::endl;
}

void test_setPassword(Channel &after_life, Client *niki, Client *beni)
{
    std::string pw1 = "123";
    std::string pw2 = "666";

    std::cout << "setPassword: ";
    // ## TEST 1 ##
    after_life.setPassword(beni, pw1, '+');
    after_life.addClient(beni, false);
    after_life.setPassword(beni, pw1, '+');
    if(pw1 != after_life.getPassword())
        std::cout << "ok ";
    else
        std::cout << "ko ";
    // ## TEST 2 ##
    after_life.setPassword(niki, pw1, '+');
    if(pw1 == after_life.getPassword())
        std::cout << "ok ";
    else
        std::cout << "ko ";
    // ## TEST 3 ##
    after_life.setPassword(niki, pw2, '+');
    if(pw2 == after_life.getPassword())
        std::cout << "ko ";
    else
        std::cout << "ok ";
    // ## TEST 4 ##
    after_life.setPassword(niki, pw2, '-');
    if(after_life.getPassword().empty() == true)
        std::cout << "ko ";
    else
        std::cout << "ok ";
    // ## TEST 5 ##
    after_life.setPassword(niki, pw1, '-');
    if(after_life.getPassword().empty() == true)
        std::cout << "ok ";
    else
        std::cout << "ko ";
}

int main()
{
    Client *niki = addUser("niki");
    Client *beni = addUser("beni");
    Channel after_life(niki, "AfterLife");

    test_isInChannel(after_life, niki, beni);
    test_rmClient(after_life, beni);
    test_setPassword(after_life, niki, beni);

    delete niki;
    delete beni;
}
