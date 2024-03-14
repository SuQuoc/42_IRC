import os
from .utils import *

#------------------Server limit------------------
def testServerLimit(server, original_directory):
	test_name = "server_limit"
	setupTest(test_name) #changes to a dir
	with open(f'{test_name}.result', 'w') as file:
		processes = start_netcat(server.host, server.port, 1016, file)
	print("Finished starting netcats")
	time.sleep(1000)
	os.chdir(original_directory)
