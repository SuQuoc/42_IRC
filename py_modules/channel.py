import os
from .utils import *

#------------------INVITE------------------
def testINVITE(server, original_directory):
	setupTest("invite")
	#errNeedMoreParams("INVITE", server)
	#errNOSUCHNICK("INVITE nonexistentuser #nonexistentchannel", server)
	#errNOSUCHCHANNEL("INVITE client0 #nonexistentchannel", server)
	#errCHANOPRIVSNEEDED("INVITE client0 #nonexistentchannel", server)
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
		(1, "TOPIC #chan"),
		(0, "TOPIC #chan :new topic"),
		(1, "TOPIC #chan"),
	]
	runMultiClientTest(test_name, 2, vector, server)


def testTOPIC(server, original_directory):
	setupTest("topic")
	#errNeedMoreParams("TOPIC", server)
	#errNOSUCHCHANNEL("TOPIC #nonexistentchannel", server)
	#errNOTONCHANNEL("TOPIC #nonexistentchannel", server)
	#errCHANOPRIVSNEEDED("TOPIC #nonexistentchannel", server)
	rplTopic(server)
	os.chdir(original_directory)