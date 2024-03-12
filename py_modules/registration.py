
import os
from .utils import *

#------------------PASS------------------
def errWrongPassword(server):
	vector = [
		(0, "PASS wrongPW"),
		#(0, "PASS 2ndWrongPW"), #connection is lostand results in broken pipe in csript
	]
	runMultiNonRegisClientTest("errWrongPassword", 1, vector, server)

def testPASS(server, original_directory):
	setupTest("pass")
	errWrongPassword(server)
	errNeedMoreParamsNotRegis("PASS", server)
	errNeedMoreParams("PASS", server)
	errAlreadyRegisteredPASS(server)
	os.chdir(original_directory)

#------------------NICK------------------
def errErroneusNicknameNotRegis(server):
	test_name = "errErroneusNicknameNotRegis"
	vector = [
		(0, "PASS " + server.pw),
		(0, "NICK clie,nt0"),
		(0, "NICK clie@nt0"),
		(0, "NICK clie!nt0"),
		(0, "NICK client0 TEXT"),
		(0, "USER user0 host0 serv0 real0"),]
	runMultiNonRegisClientTest(test_name, 1, vector, server)

def errErroneusNickname(server):
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
	runMultiClientTest(test_name, 1, vector, server)

#also checks for ERR_NICKNAMEINUSE
def changingNick(server):
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
	runMultiClientTest(test_name, 3, vector, server)

def testNICK(server, original_directory):
	setupTest("nick")	
	errNeedMoreParamsNotRegis("NICK", server) #ERR_NONICKNAMEGIVEN
	errNeedMoreParams("NICK", server) #ERR_NONICKNAMEGIVEN
	errErroneusNicknameNotRegis(server)
	errErroneusNickname(server)
	#errNicknameInUse("NICK client0") #already done by changing nick
	changingNick(server)
	os.chdir(original_directory)

#------------------USER------------------
def testUSER(server, original_directory):
	setupTest("user")
	errNeedMoreParamsNotRegis("USER", server)
	errNeedMoreParams("USER", server)
	errAlreadyRegisteredUSER(server)
	os.chdir(original_directory)
