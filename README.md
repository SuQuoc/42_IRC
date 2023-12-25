# 42_webserv

This project is about developing an IRC server in C++ 98. Internet Relay Chat or IRC is a text-based communication protocol on the Internet.
It offers real-time messaging that can be either public or private. Users can exchange direct messages and join group channels.

IRC clients connect to IRC servers in order to join channels. 
IRC servers are connected together to form a network.

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