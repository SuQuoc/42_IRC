import os
from .utils import *

#------------------Server limit------------------
def testServerLimit(server, original_directory):
	test_name = "server_limit"
	setupTest(test_name) #changes to a dir
	runMultiClientTest(test_name, 11, [(0, "PASS smth")], server) 
	os.chdir(original_directory)