# 42_IRC

This project is about developing an IRC server in C++ 98. Internet Relay Chat or IRC is a text-based communication protocol on the Internet.
It offers real-time messaging that can be either public or private. Users can exchange direct messages and join group channels.

IRC clients connect to IRC servers in order to join channels. 
IRC servers are connected together to form a network.

## IRC protocol
### Server
- All communication between clients is relayed by the
   server(s). --> no direct communication

### Channel
- a channel cease to exist if no user is in it
- a channel has to keep track of all its users and relay the message from one to all others
### Messages
- max length 512 chars ending with a Carriage Return - Line Feed pair ["\r\n"), so 510 useable chars
 ### Nicknames
- Each user is distinguished from other users by a unique nickname
- having a maximum length of nine (9) characters.

# IRC client of choice for testing
- HexChat


# Sources
- server protocol: https://datatracker.ietf.org/doc/html/rfc2813#section-2.2
- client protocol: https://datatracker.ietf.org/doc/html/rfc2812
- http://chi.cs.uchicago.edu/chirc/irc_examples.html
- https://www.geeksforgeeks.org/socket-programming-cc/
