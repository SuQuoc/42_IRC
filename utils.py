
import subprocess
import socket
import time

def get_ip_address():
	hostname = socket.gethostname()    # get local machine name
	IP = socket.gethostbyname(hostname)    # get IP address of that hostname
	print(f"The IP address of this machine is: {IP}")
	return IP

# ---------------------UTILS---------------------
def sendMsg(process, message):
	process.stdin.write((message + "\r\n").encode())
	process.stdin.flush()

def start_netcat(host, port, n, filename):
	processes = []
	for _ in range(n):
		process = subprocess.Popen(["nc", host, str(port)], stdin=subprocess.PIPE, stdout=filename)
		processes.append(process)
	return processes

def quitAllNetcats(processes):
	quit_message = "QUIT\r\n"
	for process in reversed(processes):
		process.stdin.write(quit_message.encode())
		process.stdin.flush()
		print("QUITTING")
		""" process.communicate(quit_message.encode()) """
		time.sleep(0.5)


def	registerClients(processes, password):
	i = 0
	pass_msg = "PASS " + password
	for process in processes:
		nickname = "client" + str(i)
		username = "user" + str(i)
		hostname = "host" + str(i)
		servername = "serv" + str(i)
		realname = "real " + str(i)

		nick_msg = "NICK " + nickname
		user_msg = "USER " + username + " " + hostname + " " + servername + " :" + realname
		
		sendMsg(process, pass_msg)
		sendMsg(process, nick_msg)
		sendMsg(process, user_msg)
		time.sleep(1)
		i+=1


# ---------------------IRC UTILS---------------------
def joinChannel(process, channel):
	join_msg = "JOIN " + channel
	sendMsg(process, join_msg)