#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>

#include <iostream>
#include <iterator>
#include <cstring> //only strerror()
#include <sstream>
#include <cstdlib>
#include <string>
#include <map>

void check_host_name(int hostname) 
{ //This function returns host name for local computer
   if (hostname == -1) {
      perror("gethostname");
      exit(1);
   }
}
void check_host_entry(struct hostent * hostentry) { //find host info from host name
   if (hostentry == NULL){
      perror("gethostbyname");
      exit(1);
   }
}
void IP_formatter(char *IPbuffer) { //convert IP string to dotted decimal format
   if (NULL == IPbuffer) {
      perror("inet_ntoa");
      exit(1);
   }
}


int main() 
{
   char host[256];
   char *IP;
   struct hostent *host_entry;
   int hostname;
   hostname = gethostname(host, sizeof(host)); //find the host name
   check_host_name(hostname);
   host_entry = gethostbyname(host); //find host information
   check_host_entry(host_entry);
   IP = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0])); //Convert into IP string
   printf("Current Host Name: %s\n", host);
   printf("Host IP: %s\n", IP);


   std::cout << "----------------------------------------------" << std::endl;
   struct addrinfo hints, *result, *ptr;
   int status;
   char ipstr[INET6_ADDRSTRLEN];

   // Set up hints to filter results
   std::memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_UNSPEC;    // Allow IPv4 or IPv6
   hints.ai_socktype = SOCK_STREAM;
   hints.ai_flags = AI_PASSIVE;    // Use my IP address

   // Get address information
   if ((status = getaddrinfo(nullptr, "0", &hints, &result)) != 0) {
      std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
      return 1;
   }

   // Loop through results and print IP addresses
   for (ptr = result; ptr != nullptr; ptr = ptr->ai_next) {
      void *addr;
      std::string ipver;

      // Get the pointer to the address
      if (ptr->ai_family == AF_INET) { // IPv4
         struct sockaddr_in *ipv4 = reinterpret_cast<struct sockaddr_in*>(ptr->ai_addr);
         addr = &(ipv4->sin_addr);
         ipver = "IPv4";
      } else { // IPv6
         struct sockaddr_in6 *ipv6 = reinterpret_cast<struct sockaddr_in6*>(ptr->ai_addr);
         addr = &(ipv6->sin6_addr);
         ipver = "IPv6";
      }

      // Convert the IP address to a string
      inet_ntop(ptr->ai_family, addr, ipstr, sizeof(ipstr));

      std::cout << ipver << " Address: " << ipstr << std::endl;
   }

   // Free the linked list after use
   freeaddrinfo(result);


   std::cout << "----------------------------------------------" << std::endl;
   char myIP[16];
   unsigned int myPort;
   struct sockaddr_in server_addr, my_addr;
   int sockfd;

   sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
   // Get my ip address and port
   bzero(&my_addr, sizeof(my_addr));
   socklen_t len = sizeof(my_addr);
   getsockname(sockfd, (struct sockaddr *) &my_addr, &len);
   inet_ntop(AF_INET, &my_addr.sin_addr, myIP, sizeof(myIP));
   myPort = ntohs(my_addr.sin_port);

   printf("Local ip address: %s\n", myIP);
   printf("Local port : %u\n", myPort);


   return 0;


}