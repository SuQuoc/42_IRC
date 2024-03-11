import subprocess
import socket
import filecmp
import signal
import os
import time
from colorama import Fore, Style
import utils

#------------------PASS------------------
def errWrongPassword():
	vector = [
		(0, "PASS wrongPW"),
		#(0, "PASS 2ndWrongPW"), #connection is lostand results in broken pipe in csript
	]
	runMultiNonRegisClientTest("errWrongPassword", 1, vector)

def testPASS():
	setupTest("pass")
	errWrongPassword()
	errNeedMoreParamsNotRegis("PASS")
	errNeedMoreParams("PASS")
	errAlreadyRegisteredPASS()
	os.chdir(original_directory)

testPASS()