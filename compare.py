import subprocess
import socket
import filecmp
import signal
import os
import time
from colorama import Fore, Style

def print_file_contents(filename):
    with open(filename, 'r') as file:
        contents = file.read()
    print(contents)

original_directory = os.getcwd()
os.chdir("py_tests/privmsg")

test_name = "ERR_NORECIPIENT"
if filecmp.cmp(f'{test_name}_result.txt', f'{test_name}_expected.txt', shallow=False):
	print(f"{test_name}: ✅")
else:
	print(f"{test_name}: ❌")

os.chdir(original_directory)
subprocess.run(["python3", "testscript.py"])
