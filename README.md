# 42_IRC

This project is about developing an IRC server in C++ 98. Internet Relay Chat or IRC is a text-based communication protocol on the Internet.
It offers real-time messaging that can be either public or private. Users can exchange direct messages and join group channels.

IRC clients connect to IRC servers in order to join channels. 
IRC servers are connected together to form a network.

## IRC protocol
- All communication between clients is relayed by the
   server(s). --> no direct communication
- a channel cease to exist if no user is in it
- a channel has to keep track of all its users and relay the message from one to all others
- 

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

# Testing
- we should be able to connect from different pc's belongig to the 42 network? maybe even our phones, if we are connected to the wifi?