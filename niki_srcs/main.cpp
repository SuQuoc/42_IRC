#include "Client.hpp"
#include "Channel.hpp"

int main()
{
    Client niki(3, "niki", "niki");
    Client fio(4, "fio", "fio");
    Channel six(&niki, "666");
    
    six.addClient(&fio);
    std::cout << niki.getChannels()[0]->getName() << std::endl;
    return 0;
}
