import os
from .utils import *

#------------------Server limit------------------
def testServerLimit(server, original_directory):
	test_name = "server_limit"
	setupTest(test_name) #changes to a dir
	for _ in range(1100):
		process = subprocess.Popen(["nc", server.host, str(6667)], stdin=subprocess.PIPE)

	time.sleep(1000)
	#runMultiClientTest(test_name, 1100, [(0, "PASS smth")], server) 
	os.chdir(original_directory)
