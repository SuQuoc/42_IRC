import os
from .utils import *

#------------------INVITE------------------
def userOnChannel(server):
	test_name = "userOnChannel"
	vector = [
		(0, "JOIN #chan"),
		(1, "JOIN #chan"),
		(0, "INVITE client1 #chan"),
	]
	runMultiClientTest(test_name, 2, vector, server)

def testINVITE(server, original_directory):
	setupTest("invite")
	errNeedMoreParams("INVITE", server)
	errNOSUCHNICK("INVITE nonexistentuser #nonexistentchannel", server)
	errNOTONCHANNEL("INVITE client0 #chan", server)
	errCHANOPRIVSNEEDED("INVITE client0 #chan", server)
	userOnChannel(server)
	os.chdir(original_directory)

#------------------MODE------------------
def testMODE(server, original_directory):
	setupTest("mode")
	#errNeedMoreParams("MODE", server)
	#errNOSUCHNICK("MODE nonexistentuser", server)
	#errNOSUCHCHANNEL("MODE #nonexistentchannel", server)
	#errCHANOPRIVSNEEDED("MODE #nonexistentchannel", server)
	os.chdir(original_directory)

#------------------TOPIC------------------
def rplTopic(server):
	test_name = "rplTopic"
	vector = [
		(0, "JOIN #chan"),
		(1, "JOIN #chan"),
		(1, "TOPIC #chan"),
		(0, "TOPIC #chan :new topic"),
		(1, "TOPIC #chan"),
	]
	runMultiClientTest(test_name, 2, vector, server)


def testTOPIC(server, original_directory):
	setupTest("topic")
	errNeedMoreParams("TOPIC", server)
	errNOSUCHCHANNEL("TOPIC #nonexistentchannel", server) #AfterNet sends ERR_NOSUCHCHANNEL
	errNOTONCHANNEL("TOPIC #chan", server) #user can see topic although not in channel --> NOT WORKING
	errCHANOPRIVSNEEDED("TOPIC #chan im not operator", server)
	rplTopic(server) #also tests RPL_NOTOPIC --> NOT WORKING missing afterlife prefix
	os.chdir(original_directory)