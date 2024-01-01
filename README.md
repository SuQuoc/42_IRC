# 42_IRC

This project is about developing an IRC server in C++ 98. Internet Relay Chat or IRC is a text-based communication protocol on the Internet.
It offers real-time messaging that can be either public or private. Users can exchange direct messages and join group channels.

IRC clients connect to IRC servers in order to join channels. 
IRC servers are connected together to form a network.

## To Do
- struktur fuer Commands die channels verwenden ueberlegen
	- soll jeder command dann den channel namen rauslesen
	- soll es davor rausgelesen werden und dann gepassed werden
	- welches der beiden ist sinnvoller und sollen die commands unterschieden werden
		zwischen denen die einen Channel nehmen und denen die keinen nehmen?


## IRC protocol
### Server
- All communication between clients is relayed by the
   server(s). --> no direct communication
- maximum length of 63 characters --> grammar: https://datatracker.ietf.org/doc/html/rfc2812#section-2.3.1

### Channel
- a channel cease to exist if no user is in it
- a channel has to keep track of all its users and relay the message from one to all others
### Messages
- messages
	- only valid prefix from client would be ":client_nickname"
	- max length 512 chars ending with a (Carriage Return - Line Feed) pair (i.e., “\r\n”) --> so 510 useable chars
		- what should happen if it exceeds? Error message back to client or truncate mesage?
- nicknames
	- unique across the server (or irc network? since we dont talk to other servers unique for our server i guess)
	- Each user is distinguished from other users by a unique nickname
   having a maximum length of nine (9) characters.
   - https://datatracker.ietf.org/doc/html/rfc2813#section-3.3.1

- server protocol: https://datatracker.ietf.org/doc/html/rfc2813#section-2.2
- client protocol: https://datatracker.ietf.org/doc/html/rfc2812

# Keep in mind
- if you try to read/recv or write/send in any file descriptor without using poll() (or equivalent), your grade will be 0

# Whats ...
- poll or select(), kqueue, or epoll?
- what are all the other functions dong?
- an I/O operation which is non-blocking?
	- read() or receive() from a socket is a blocking operation
	- read would only complete if data is available
- a non-blocking file descriptor
- Which IRC client should we choose as a reference (IT WILL BE USED DURING EVAL)
- How do we verify absolutely every possible error and issue (receiving partial data, low bandwidth,
and so forth).
- nc?
	- 

# IRC clients
- HexChat on a VM
- revolution IRC on smartphone


# Sources
- https://www.geeksforgeeks.org/socket-programming-cc/
- I/O blocking and non-blocking
	- https://www.youtube.com/watch?v=wB9tIg209-8
- Example flight time server
	- https://www.softprayog.in/programming/io-multiplexing-select-poll-epoll-in-linux
- http://chi.cs.uchicago.edu/chirc/irc_examples.html


# Testing
- we should be able to connect from different pc's belongig to the 42 network? maybe even our phones, if we are connected to the wifi?