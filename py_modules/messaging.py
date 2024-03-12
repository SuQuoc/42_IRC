
import os
from .utils import *

#------------------PRIVMSG------------------
def msgTest(server):
	vector = [
		(0, "PRIVMSG client1 :hello from client1 to you my dear client0"),
		]
	runMultiClientTest("privmsg_test", 2, vector, server)

def testPRIVMSG(server, original_directory):
	setupTest("privmsg") #testname
	errNeedMoreParams("PRIVMSG", server)
	errNORECIPIENT("PRIVMSG", server)
	#errNOTEXTTOSEND("PRIVMSG", server) #i need a user that exists or a channel
	errNOSUCHCHANNEL("PRIVMSG #nonexistentchannel", server)
	errNOSUCHNICK("PRIVMSG nonexistentuser", server)
	msgTest(server)
	os.chdir(original_directory)