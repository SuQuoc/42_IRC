import os
from .utils import *

#------------------WHO------------------
def whoWithOver512chars(server):
	test_name = "whoWithOver512chars"
	vector = []
	n_clients = 32
	for i in range(n_clients):
		vector.append((i, "JOIN #chan"))
	vector.append((0, "WHO #chan"))
	runMultiClientTest(test_name, n_clients, vector, server)
	

def testWHO(server, original_directory):
	setupTest("who")
	whoWithOver512chars(server)
	os.chdir(original_directory)

