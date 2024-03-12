
import subprocess
import socket
import time
import os
import signal
import filecmp
from colorama import Fore, Style
# from testscript import serv_host, serv_port, serv_pw

class Server:
	def __init__(self, host, port, password):
		self.host = host
		self.port = port
		self.pw = password

# ---------------------UTILS---------------------
def get_ip_address():
	hostname = socket.gethostname()    # get local machine name
	IP = socket.gethostbyname(hostname)    # get IP address of that hostname
	print(f"The IP address of this machine is: {IP}")
	return IP

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
	for process in processes:
		process.stdin.write(quit_message.encode())
		process.stdin.flush()
		""" process.communicate(quit_message.encode()) """
		time.sleep(0.2)

def sigintAllClients(processes):
	for process in processes:
		os.kill(process.pid, signal.SIGINT)

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


# ---------------------core framework?---------------------
def setupTest(test_name): #creates and changes to a dir
	print(f"{Style.BRIGHT}{Fore.YELLOW}---{test_name} test---")
	print(Style.RESET_ALL)
	dir_path = f"py_tests/{test_name}"
	if not os.path.exists(dir_path):
		os.makedirs(dir_path)
	os.chdir(f"py_tests/{test_name}")

def runMultiNonRegisClientTest(test_name, n_clients, vector, server):
	with open(f'{test_name}.result', 'w') as file:
		processes = start_netcat(server.host, server.port, n_clients, file)
		for pair in vector:
			client_id = pair[0]
			msg = pair[1]
			sendMsg(processes[client_id], msg)
			time.sleep(0.5)
		sigintAllClients(processes)
	
	if not os.path.isfile(f'{test_name}.expected'):
		open(f'{test_name}.expected', 'w').close()
	if filecmp.cmp(f'{test_name}.result', f'{test_name}.expected'):
		print(f"{test_name}: ✅")
	else:
		print(f"{test_name}: ❌")

def runMultiClientTest(test_name, n_clients, vector, server):
	with open(f'{test_name}.result', 'w') as file:
		processes = start_netcat(server.host, server.port, n_clients, file)
		registerClients(processes, server.pw)
		for pair in vector:
			client_id = pair[0]
			msg = pair[1]
			sendMsg(processes[client_id], msg)
			time.sleep(0.2)
		quitAllNetcats(processes)
		#sigintAllClients(processes)

	if not os.path.isfile(f'{test_name}.expected'):
		open(f'{test_name}.expected', 'w').close()

	if filecmp.cmp(f'{test_name}.result', f'{test_name}.expected'):
		print(f"{test_name}: ✅")
	else:
		print(f"{test_name}: ❌")

#------------------ERR CODES------------------
def errNeedMoreParamsNotRegis(msg, server):
	runMultiNonRegisClientTest("errNeedMoreParamsNotRegis", 1, [(0, msg)], server)

def errNeedMoreParams(msg, server):
	test_name = "ERR_NEEDMOREPARAMS"
	runMultiClientTest(test_name, 1, [(0, msg)], server)

def errAlreadyRegisteredPASS(server):
	test_name = "ERR_ALREADYREGISTERED"
	#doesnt lose connection because client already registered
	vector = [
		(0, "PASS"),
		#(0, "PASS " + serv_pw), #correct password
		#(0, "PASS wrong_password"), #wrong password
	]
	runMultiClientTest("errAlreadyRegisteredPASS", 1, vector, server)

def errAlreadyRegisteredUSER(server):
	test_name = "ERR_ALREADYREGISTERED"
	vector = [
		(0, "USER"),
		(0, "USER userX"),
		(0, "USER userX hostX servX reealX"),
	]
	runMultiClientTest("errAlreadyRegisteredUSER", 1, vector, server)

def errNORECIPIENT(msg, server):
	test_name = "ERR_NORECIPIENT"
	vector = [(0, msg)]
	runMultiClientTest(test_name, 1, vector, server)

def errNOTEXTTOSEND(msg, server):
	test_name = "ERR_NOTEXTTOSEND"
	vector = [(0, msg)]
	runMultiClientTest(test_name, 1, vector, server)

def errNOSUCHCHANNEL(msg, server):
	test_name = "ERR_NOSUCHCHANNEL"
	vector = [(0, msg)]
	runMultiClientTest(test_name, 1, vector, server)

def errNOSUCHNICK(msg, server):
	test_name = "ERR_NOSUCHNICK"
	vector = [(0, msg)]
	runMultiClientTest(test_name, 1, vector, server)

