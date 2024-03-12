
import os
from .utils import *

#------------------PRIVMSG------------------
def msgTest(server):
	vector = [
		(0, "PRIVMSG client1 :hello from client1 to you my dear client0"),
		]
	runMultiClientTest("privmsg_test", 2, vector, server)


def noTextToSend(server):
	vector = [
		(0, "PRIVMSG client1"),
	]
	runMultiClientTest("noTextToSend", 2, vector, server)

def testPRIVMSG(server, original_directory):
	setupTest("privmsg") #testname
	errNeedMoreParams("PRIVMSG", server)
	errNORECIPIENT("PRIVMSG", server)
	errNOSUCHCHANNEL("PRIVMSG #nonexistentchannel", server)
	errNOSUCHNICK("PRIVMSG nonexistentuser", server)
	noTextToSend(server) #i need a user that exists or a channel
	msgTest(server)
	os.chdir(original_directory)