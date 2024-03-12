
import os
from .utils import *

#------------------QUIT------------------
def testQUIT(server, original_directory):
	setupTest("quit")
	errNeedMoreParams("QUIT", server)
	os.chdir(original_directory)


#------------------KICK------------------
def noSuchNick(server):
	test_name = "noSuchNick"
	vector = [
		(0, "JOIN #chan"),
		(0, "KICK #chan noNick"),
		]
	runMultiClientTest("noSuchNick", 1, vector, server)

def testKICK(server, original_directory):
	setupTest("kick")
	errNeedMoreParams("KICK", server)
	errNOSUCHCHANNEL("KICK #noSuchCh nickname", server)
	noSuchNick(server)
	errNOTONCHANNEL("KICK #chan client0", server)
	errCHANOPRIVSNEEDED("KICK #chan client0", server)
	os.chdir(original_directory)