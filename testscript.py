
import os
import re
from colorama import Fore, Style
from py_modules import get_ip_address, Server
from py_modules import testPASS, testNICK, testUSER, testPRIVMSG, testJOIN, testPART, testQUIT, testKICK, testINVITE, testMODE, testTOPIC, testOPER, testServerLimit


# Start netcat
#processes = start_netcat(serv_host, serv_port, n, filename)
#killAllProcesses(processes)

def convert_to_crlf(filepath):
    """Converts the line endings of a file from LF to CRLF."""
    with open(filepath, 'rb') as file:
        content = file.read()

    content = content.replace(b'\n', b'\r\n')

    with open(filepath, 'wb') as file:
        file.write(content)

def convert_directory_to_crlf(dirpath):
    """Converts the line endings of all files in a directory from LF to CRLF."""
    for dirpath, dirnames, filenames in os.walk(dirpath):
        for filename in filenames:
            filepath = os.path.join(dirpath, filename)
            convert_to_crlf(filepath)

def replace_ip_in_file(filepath, new_ip):
	with open(filepath, 'r') as file:
		content = file.read()

	# Replace the string that starts with @10. and ends with a space
	new_ip = "@" + new_ip
	content = re.sub(r'@10\.\S+', new_ip, content)

	with open(filepath, 'w') as file:
		file.write(content)

def setupExpectedFiles(dir_to_process):
	for dirpath, dirnames, filenames in os.walk(dir_to_process):
		for filename in filenames:
			# Skip files that don't end with .expected
			if not filename.endswith('.expected'):
				continue	
			# Replace the string in the file
			filepath = os.path.join(dirpath, filename)
			replace_ip_in_file(filepath, host)
			convert_to_crlf(filepath) #making sure that all files have CRLF line endings


# Global variables
original_directory = os.getcwd()
test_dir = "py_tests"
# Get user input
# serv_host = input("Enter server ip: ")
# n_connections = int(input("Enter number of connections: "))
# serv_port = int(input("Enter server port: "))
# serv_pw = input("Enter server password: ")

# -------main----------
host = get_ip_address()
port = 6667
pw = "pw1234567"
server = Server(host, port, pw)

setupExpectedFiles(test_dir)

testPASS(server , original_directory)
testNICK(server, original_directory)
testUSER(server, original_directory)
testPRIVMSG(server, original_directory)
testJOIN(server, original_directory)
testPART(server, original_directory)
testQUIT(server, original_directory)
testKICK(server, original_directory)
testINVITE(server, original_directory)
testMODE(server, original_directory)
testTOPIC(server, original_directory)
testOPER(server, original_directory) #also tests KILL
#testServerLimit(server, original_directory)
