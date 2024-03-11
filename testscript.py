
import subprocess
import socket
import filecmp
import signal
import os
import time
from colorama import Fore, Style
from utils import get_ip_address, sendMsg, start_netcat, quitAllNetcats, registerClients

# ---------------------core framework?---------------------
def setupTest(test_name): #creates and changes to a dir
	print(f"{Style.BRIGHT}{Fore.YELLOW}---{test_name} test---")
	print(Style.RESET_ALL)
	dir_path = f"py_tests/{test_name}"
	if not os.path.exists(dir_path):
		os.makedirs(dir_path)
	os.chdir(f"py_tests/{test_name}")

def sigintAllClients(processes):
	for process in processes:
		os.kill(process.pid, signal.SIGINT)

def runMultiNonRegisClientTest(test_name, n_clients, vector):
	with open(f'{test_name}.result', 'w') as file:
		processes = start_netcat(serv_host, serv_port, n_clients, file)
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

def runMultiClientTest(test_name, n_clients, vector):
	with open(f'{test_name}.result', 'w') as file:
		processes = start_netcat(serv_host, serv_port, n_clients, file)
		registerClients(processes, serv_pw)
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
def errNeedMoreParamsNotRegis(msg):
	runMultiNonRegisClientTest("errNeedMoreParamsNotRegis", 1, [(0, msg)])

def errNeedMoreParams(msg):
	test_name = "ERR_NEEDMOREPARAMS"
	runMultiClientTest(test_name, 1, [(0, msg)])

def errAlreadyRegisteredPASS():
	test_name = "ERR_ALREADYREGISTERED"
	#doesnt lose connection because client already registered
	vector = [
		(0, "PASS"),
		#(0, "PASS " + serv_pw), #correct password
		#(0, "PASS wrong_password"), #correct password
	]
	runMultiClientTest("errAlreadyRegisteredPASS", 1, vector)

def errAlreadyRegisteredUSER():
	test_name = "ERR_ALREADYREGISTERED"
	vector = [
		(0, "USER"),
		(0, "USER userX"),
		(0, "USER userX hostX servX reealX"),
	]
	runMultiClientTest("errAlreadyRegisteredUSER", 1, vector)


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
def errWrongPassword():
	vector = [
		(0, "PASS wrongPW"),
		#(0, "PASS 2ndWrongPW"), #connection is lostand results in broken pipe in csript
	]
	runMultiNonRegisClientTest("errWrongPassword", 1, vector)

def testPASS():
	setupTest("pass")
	errWrongPassword()
	errNeedMoreParamsNotRegis("PASS")
	errNeedMoreParams("PASS")
	errAlreadyRegisteredPASS()
	os.chdir(original_directory)

#------------------NICK------------------
def errErroneusNicknameNotRegis():
	test_name = "errErroneusNicknameNotRegis"
	vector = [
		(0, "PASS " + serv_pw),
		(0, "NICK clie,nt0"),
		(0, "NICK clie@nt0"),
		(0, "NICK clie!nt0"),
		(0, "NICK client0 TEXT"),
		(0, "USER user0 host0 serv0 real0"),]
	runMultiNonRegisClientTest(test_name, 1, vector)

def errErroneusNickname():
	test_name = "ERR_ERRONEUSNICKNAME"
	vector = [
		(0, "NICK 123456789abc"),
		(0, "NICK 	"),
		(0, "NICK \n"),
		(0, "NICK clie,nt0"),
		(0, "NICK clie@nt0"),
		(0, "NICK clie!nt0"),
		(0, "NICK space nt0"),
		(0, "NICK space nt0"), #changing nick to the same nick
		(0, "NICK space0 1 2"),
		(0, "NICK     Nspace   1   2"),
		(0, "NICK"),
		]
	runMultiClientTest(test_name, 1, vector)

#also checks for ERR_NICKNAMEINUSE
def changingNick():
	test_name = "changingNick"
	vector = [
		(1, "PRIVMSG client0 :You should see this message"),
		(0, "NICK newNick"),
		(1, "PRIVMSG newNick :if you see this, it means that the nick was changed successfully"),
		(1, "PRIVMSG client0 :'No such nick/channel' should be sent"),
		(2, "NICK client0"),
		(2, "NICK newNick"), #nick already in use
		(1, "PRIVMSG client0 :if you see this, it means that the nick was changed successfully"),
		]
	runMultiClientTest(test_name, 3, vector)

def testNICK():
	setupTest("nick")	
	errNeedMoreParamsNotRegis("NICK") #ERR_NONICKNAMEGIVEN
	errNeedMoreParams("NICK") #ERR_NONICKNAMEGIVEN
	errErroneusNicknameNotRegis()
	errErroneusNickname()
	#errNicknameInUse("NICK client0") #already done by changing nick
	changingNick()
	os.chdir(original_directory)


#------------------USER------------------
	
def testUSER():
	setupTest("user")
	errNeedMoreParamsNotRegis("USER")
	errNeedMoreParams("USER")
	errAlreadyRegisteredUSER()
	os.chdir(original_directory)
	
#------------------PRIVMSG------------------
def msgTest():
	vector = [
		(0, "PRIVMSG client1 :hello from client1 to you my dear client0"),
		]
	runMultiClientTest("privmsg_test", 2, vector)

def testPRIVMSG():
	setupTest("privmsg") #testname
	errNeedMoreParams("PRIVMSG")
	errNORECIPIENT("PRIVMSG")
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

def fullChannel():
	test_name = "fullChannel"
	vector = [
		(0, "JOIN #chan"),
		(1, "JOIN #chan"),
		(2, "JOIN #chan"),
		(3, "JOIN #chan"),
		(4, "JOIN #chan"),
		(5, "JOIN #chan"),
		(6, "JOIN #chan"),
		(7, "JOIN #chan"),
		(8, "JOIN #chan"),
		(9, "JOIN #chan"), #channel is full - 10 clients
		(10, "JOIN #chan"), #trying to join a full channel
	]
	runMultiClientTest(test_name, 11, vector)

def joinTooManyChannelsWithList():
	test_name = "joinTooManyChannelsWithList"
	vector = [
		(0, "JOIN #chan0,#chan1,#chan2,#chan3,#chan4,#chan5,#chan6,#chan7,#chan8,#chan9,#chan10"), #joining 11 channels
		(0, "JOIN #chan10,#chan11,"), #only joined 10 before due to list limit, now error should be triggered
	]
	runMultiClientTest(test_name, 1, vector)

def inviteOnlyChannel():
	test_name = "inviteOnlyChannel"
	vector = [
		(0, "JOINT #chan"),
		# (0, "MODE #chan +i"),
		# (1, "JOIN #chan"), #cant join
		# (0, "INVITE client1 #chan"),
		# (1, "JOIN #chan"), #can join now
		]
	runMultiClientTest(test_name, 2, vector)

def inviteOnlyChannelOrder():
	test_name = "inviteOnlyChannelOrder"
	vector = [
		(0, "JOIN #chan"),
		(0, "INVITE client1 #chan"),
		(0, "MODE #chan +i"),
		(1, "JOIN #chan"), #can join now
		]
	runMultiClientTest(test_name, 2, vector)
	
def badChannelKey():
	test_name = "badChannelKey"
	vector = [
		(0, "JOIN #chan"),
		(0, "MODE #chan +k Key"),
		(1, "JOIN #chan"), #cant join
		(1, "JOIN #chan wrong key"), #cant join
		(1, "JOIN #chan key"), #cant join case sensitive?
		(1, "JOIN #chan Key"), #cant join case sensitive?
		]
	runMultiClientTest(test_name, 2, vector)

def testJOIN():
	setupTest("join")
	#errNeedMoreParams("JOIN")
	#errNOSUCHCHANNEL("JOIN #nonexistentchannel")
	
	joiningTooManyChannels() #1 client trying to join 11 channels
	joinTooManyChannelsWithList() #1 client trying to join 11 channels in a list
	fullChannel() #11 clients trying to join  
	inviteOnlyChannel()
	inviteOnlyChannelOrder() #weird order
	badChannelKey()
	os.chdir(original_directory)

#------------------PART------------------
def partingChannel():
	test_name = "partingChannel"
	vector = [
		(0, "JOIN #chan"),
		(1, "JOIN #chan"),
		(0, "PART #chan"),
		(0, "PART #chan"),
		(1, "MODE #chan +i"),
		]
	runMultiClientTest(test_name, 2, vector)

def testPART():
	setupTest("part")
	#errNeedMoreParams("PART")
	#errNOSUCHCHANNEL("PART #nonexistentchannel")
	partingChannel()
	os.chdir(original_directory)

#------------------QUIT------------------
def testQUIT():
	setupTest("quit")
	#errNeedMoreParams("QUIT")
	os.chdir(original_directory)


#------------------KICK------------------
def testKICK():
	setupTest("kick")
	#errNeedMoreParams("KICK")
	#errNOSUCHCHANNEL("KICK #nonexistentchannel")
	#errNOSUCHNICK("KICK nonexistentuser")
	#errNOTONCHANNEL("KICK #nonexistentchannel nonexistentuser")
	#errCHANOPRIVSNEEDED("KICK #nonexistentchannel nonexistentuser")
	os.chdir(original_directory)

#------------------INVITE------------------
def testINVITE():
	setupTest("invite")
	#errNeedMoreParams("INVITE")
	#errNOSUCHNICK("INVITE nonexistentuser #nonexistentchannel")
	#errNOSUCHCHANNEL("INVITE client0 #nonexistentchannel")
	#errCHANOPRIVSNEEDED("INVITE client0 #nonexistentchannel")
	os.chdir(original_directory)

#------------------MODE------------------
def testMODE():
	setupTest("mode")
	#errNeedMoreParams("MODE")
	#errNOSUCHNICK("MODE nonexistentuser")
	#errNOSUCHCHANNEL("MODE #nonexistentchannel")
	#errCHANOPRIVSNEEDED("MODE #nonexistentchannel")
	os.chdir(original_directory)

#------------------TOPIC------------------
def rplTopic():
	test_name = "rplTopic"
	vector = [
		(0, "JOIN #chan"),
		(1, "TOPIC #chan"),
		(0, "TOPIC #chan :new topic"),
		(1, "TOPIC #chan"),
	]
	runMultiClientTest(test_name, 2, vector)


def testTOPIC():
	setupTest("topic")
	#errNeedMoreParams("TOPIC")
	#errNOSUCHCHANNEL("TOPIC #nonexistentchannel")
	#errNOTONCHANNEL("TOPIC #nonexistentchannel")
	#errCHANOPRIVSNEEDED("TOPIC #nonexistentchannel")
	rplTopic()
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
		(0, "KILL client0: cant do i have no priviliges"), #ERR_NOPRIVILEGES
		(0, "OPER 10.14.3.10 setOpPW"), #--> You are now an IRC operator
		(0, "KILL"), #ERR_NEEDMOREPARAMS
		(0, "KILL NoNick "), #ERR_NOSUCHNICK
		(0, "PRIVMSG client1 :you should see this cuz client 1 is alive and was not kiled"),
		(1, "PRIVMSG client0 :yes i am alive"),
		(0, "KILL client1 :successfull kill of client1"),
		(0, "PRIVMSG client1 :'No such nick/channel' should be sent"),
		(0, "KILL client0 :killing myself??"), #problematic cuz running runMultiClientTest after, which sends a QUIT for all users  
		(2, "PRIVMSG client0 :'No such nick/channel' should be sent"),
	]
	runMultiClientTest(test_name, 3, vector)

def testOPER():
	print(f"{Style.BRIGHT}{Fore.YELLOW}---OPER + KILL TESTS---")
	print(Style.RESET_ALL)
	os.chdir("py_tests/oper")
	errNeedMoreParams("OPER")
	wrongOperHost()
	wrongOperPw()
	kill()
	os.chdir(original_directory)

#------------------Server limit------------------
def testServerLimit():
	test_name = "server_limit"
	setupTest(test_name) #changes to a dir
	runMultiClientTest(test_name, 11, [(0, "PASS smth")])
	os.chdir(original_directory)

# Start netcat
#processes = start_netcat(serv_host, serv_port, n, filename)
#killAllProcesses(processes)


# Global variables
serv_host = get_ip_address()
serv_port = 6667
serv_pw = "pw1234567"
original_directory = os.getcwd()
test_dir = "py_tests"
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
#testServerLimit()
