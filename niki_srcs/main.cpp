#include "Client.hpp"
#include "Channel.hpp"

void create_pointer(Channel &six)
{
    Client *fio = new Client(4, "fio", "fio");

    six.addClient(fio);
}

int main()
{
    Client niki(3, "niki", "niki");
    Channel six(&niki, "666");
    create_pointer(six);
    std::cout << six.getMembers().find("fio")->second->getNickName() << std::endl;
    /* six.addClient(&fio); */
    /* std::cout << six. << std::endl; */
    return 0;
}
