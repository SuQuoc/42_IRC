
import os
from .utils import *

#------------------QUIT------------------
def testQUIT(server, original_directory):
	setupTest("quit")
	#errNeedMoreParams("QUIT", server)
	os.chdir(original_directory)


#------------------KICK------------------
def testKICK(server, original_directory):
	setupTest("kick")
	errNeedMoreParams("KICK", server)
	errNOSUCHCHANNEL("KICK #nonexistentchannel", server)
	errNOSUCHNICK("KICK nonexistentuser", server)
	errNOTONCHANNEL("KICK #nonexistentchannel nonexistentuser", server)
	errCHANOPRIVSNEEDED("KICK #nonexistentchannel nonexistentuser", server)
	os.chdir(original_directory)