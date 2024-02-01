
import subprocess
import socket
import filecmp
import signal
import os
import time
from colorama import Fore, Style

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
        process.communicate(quit_message.encode())

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
		time.sleep(0.5)
		i+=1


# ---------------------IRC UTILS---------------------
def joinChannel(process, channel):
	join_msg = "JOIN " + channel
	sendMsg(process, join_msg)

# ---------------------core framework?---------------------
def runMultiClientTest(test_name, n_clients, vector):
	with open(f'{test_name}.result', 'w') as file:
		processes = start_netcat(serv_host, serv_port, n_clients, file)
		registerClients(processes, serv_pw)
		for pair in vector:
			client_id = pair[0]
			msg = pair[1]
			sendMsg(processes[client_id], msg)
			time.sleep(0.5)
		quitAllNetcats(processes)

	if not os.path.isfile(f'{test_name}.expected'):
		open(f'{test_name}.expected', 'w').close()

	if filecmp.cmp(f'{test_name}.result', f'{test_name}.expected'):
		print(f"{test_name}: ✅")
	else:
		print(f"{test_name}: ❌")


def runSingleClientTest(test_name, msg):
	with open(f'{test_name}.result', 'w') as file:
		processes = start_netcat(serv_host, serv_port, 1, file)
		registerClients(processes, serv_pw)
		sendMsg(processes[0], msg)
		msg = "QUIT\r\n"
		processes[0].communicate(msg.encode())
		processes[0].wait()

	if not os.path.isfile(f'{test_name}.expected'):
		open(f'{test_name}.expected', 'w').close()

	if filecmp.cmp(f'{test_name}.result', f'{test_name}.expected'):
		print(f"{test_name}: ✅")
	else:
		print(f"{test_name}: ❌")


# ---------------------TESTS---------------------
#------------------ERR CODES------------------
def errNeedMoreParams(msg):
	test_name = "ERR_NEEDMOREPARAMS"
	runMultiClientTest(test_name, 1, [(0, msg)])

def errNORECIPIENT(msg):
	test_name = "ERR_NORECIPIENT"
	runSingleClientTest(test_name, msg)

def errNOTEXTTOSEND(msg):
	test_name = "ERR_NOTEXTTOSEND"
	runSingleClientTest(test_name, msg)

def errNOSUCHCHANNEL(msg):
	test_name = "ERR_NOSUCHCHANNEL"
	runSingleClientTest(test_name, msg)	

def errNOSUCHNICK(msg):
	test_name = "ERR_NOSUCHNICK"
	runSingleClientTest(test_name, msg)

#------------------PASS------------------
def testPASS():
	print(f"{Style.BRIGHT}{Fore.YELLOW}---PASS TESTS---")
	print(Style.RESET_ALL)
	os.chdir("py_tests/pass")
	errNeedMoreParams("PASS")
	#errAlreadyRegistered("PASS")
	#errAlreadyRegistered("PASS pw1234567")
	#errWrongPassword("PASS wrongpw")
	#errWrongPassword("PASS pw1234567 wrongpw")
	#errWrongPassword("PASS pw1234567)
	os.chdir(original_directory)

#------------------NICK------------------
def testNICK():
	print(f"{Style.BRIGHT}{Fore.YELLOW}---NICK TESTS---")
	print(Style.RESET_ALL)
	os.chdir("py_tests/nick")
	errNeedMoreParams("NICK")
	#errNoNicknameGiven("NICK client0")
	#errErroneusNickname("NICK client0")
	#errNicknameInUse("NICK client0")
	os.chdir(original_directory)


#------------------USER------------------
def testUSER():
	print(f"{Style.BRIGHT}{Fore.YELLOW}---USER TESTS---")
	print(Style.RESET_ALL)
	os.chdir("py_tests/user")
	errNeedMoreParams("USER")
	#errAlreadyRegistered("USER")
	#errAlreadyRegistered("USER user1 host1 serv1 :real1")
	os.chdir(original_directory)
	
#------------------PRIVMSG------------------
def msgTest():
	vector = [
		(0, "PRIVMSG client1 :hello from client1 to you my dear client0"),
		]
	runMultiClientTest("privmsg_test", 2, vector)

def testPRIVMSG():
	print(f"{Style.BRIGHT}{Fore.YELLOW}---PRIVMSG TESTS---")
	print(Style.RESET_ALL)
	os.chdir("py_tests/privmsg")
	errNeedMoreParams("PRIVMSG")
	#errNORECIPIENT("PRIVMSG")
	#errNOTEXTTOSEND("PRIVMSG") #i need a user that exists or a channel
	errNOSUCHCHANNEL("PRIVMSG #nonexistentchannel")
	errNOSUCHNICK("PRIVMSG nonexistentuser")
	msgTest()
	os.chdir(original_directory)


#------------------JOIN------------------
def joiningTooManyChannels():
	test_name = "joiningTooManyChannels"
	
	vector = []
	for i in range(11):
		join_msg = "JOIN #chan" + str(i)
		vector.append((0, join_msg))		
	runMultiClientTest(test_name, 1, vector)

def testJOIN():
	print(f"{Style.BRIGHT}{Fore.YELLOW}---JOIN TESTS---")
	print(Style.RESET_ALL)
	os.chdir("py_tests/join")
	errNeedMoreParams("JOIN")
	errNOSUCHCHANNEL("JOIN #nonexistentchannel")
	joiningTooManyChannels()
	os.chdir(original_directory)

#------------------PART------------------
def testPART():
	print(f"{Style.BRIGHT}{Fore.YELLOW}---PART TESTS---")
	print(Style.RESET_ALL)
	os.chdir("py_tests/part")
	errNeedMoreParams("PART")
	errNOSUCHCHANNEL("PART #nonexistentchannel")
	os.chdir(original_directory)

#------------------QUIT------------------
def testQUIT():
	print(f"{Style.BRIGHT}{Fore.YELLOW}---QUIT TESTS---")
	print(Style.RESET_ALL)
	os.chdir("py_tests/quit")
	errNeedMoreParams("QUIT")
	os.chdir(original_directory)


#------------------KICK------------------
def testKICK():
	print(f"{Style.BRIGHT}{Fore.YELLOW}---KICK TESTS---")
	print(Style.RESET_ALL)
	os.chdir("py_tests/kick")
	errNeedMoreParams("KICK")
	errNOSUCHCHANNEL("KICK #nonexistentchannel")
	errNOSUCHNICK("KICK nonexistentuser")
	#errNOTONCHANNEL("KICK #nonexistentchannel nonexistentuser")
	#errCHANOPRIVSNEEDED("KICK #nonexistentchannel nonexistentuser")
	os.chdir(original_directory)

#------------------INVITE------------------
def testINVITE():
	print(f"{Style.BRIGHT}{Fore.YELLOW}---INVITE TESTS---")
	print(Style.RESET_ALL)
	os.chdir("py_tests/invite")
	errNeedMoreParams("INVITE")
	errNOSUCHNICK("INVITE nonexistentuser #nonexistentchannel")
	errNOSUCHCHANNEL("INVITE client0 #nonexistentchannel")
	#errCHANOPRIVSNEEDED("INVITE client0 #nonexistentchannel")
	os.chdir(original_directory)

#------------------MODE------------------
def testMODE():
	print(f"{Style.BRIGHT}{Fore.YELLOW}---MODE TESTS---")
	print(Style.RESET_ALL)
	os.chdir("py_tests/mode")
	errNeedMoreParams("MODE")
	errNOSUCHNICK("MODE nonexistentuser")
	errNOSUCHCHANNEL("MODE #nonexistentchannel")
	#errCHANOPRIVSNEEDED("MODE #nonexistentchannel")
	os.chdir(original_directory)

#------------------TOPIC------------------
def testTOPIC():
	print(f"{Style.BRIGHT}{Fore.YELLOW}---TOPIC TESTS---")
	print(Style.RESET_ALL)
	os.chdir("py_tests/topic")
	errNeedMoreParams("TOPIC")
	errNOSUCHCHANNEL("TOPIC #nonexistentchannel")
	#errNOTONCHANNEL("TOPIC #nonexistentchannel")
	#errCHANOPRIVSNEEDED("TOPIC #nonexistentchannel")
	os.chdir(original_directory)
#------------------OPER + KILL------------------
def wrongOperHost():
	test_name = "wrongOperHost"
	vector = [(0, "OPER 7.7.7.7 setOpPW")]
	runMultiClientTest(test_name, 1, vector)

def wrongOperPw():
	test_name = "wrongOperPw"
	vector = [(0, "OPER 10.14.3.10 wrongOperPw")]
	runMultiClientTest(test_name, 1, vector)

def kill():
	test_name = "oper_kill"
	
	vector = [
		(0, "KILL client0: bye bye"),
		(0, "OPER 10.14.3.10 setOpPW"),
		(0, "KILL client0: bye bye"),
	]
	runMultiClientTest(test_name, 2, vector)

def testOPER():
	print(f"{Style.BRIGHT}{Fore.YELLOW}---OPER + KILL TESTS---")
	print(Style.RESET_ALL)
	os.chdir("py_tests/oper")
	errNeedMoreParams("OPER")
	wrongOperHost()
	wrongOperPw()
	kill()
	os.chdir(original_directory)

# Start netcat
#processes = start_netcat(serv_host, serv_port, n, filename)
#killAllProcesses(processes)


# Global variables
serv_host = get_ip_address()
serv_port = 6667
serv_pw = "pw1234567"
original_directory = os.getcwd()

# Get user input
# serv_host = input("Enter server ip: ")
# n_connections = int(input("Enter number of connections: "))
# serv_port = int(input("Enter server port: "))
# serv_pw = input("Enter server password: ")

# -------main----------
testPASS()
testNICK()
testUSER()
testPRIVMSG()
testJOIN()
testPART()
testQUIT()
testKICK()
testINVITE()
testMODE()
testTOPIC()
testOPER() #also tests KILL
