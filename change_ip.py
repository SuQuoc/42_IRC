
import os
import fileinput
import socket

def get_ip_address():
    hostname = socket.gethostname()    # get local machine name
    IP = socket.gethostbyname(hostname)    # get IP address of that hostname
    print(f"The IP address of this machine is: {IP}")
    return IP

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


# Get your local IP address
ip_address = get_ip_address()

# Ask the user for the string to replace
string_to_replace = input("Enter the IP-address to replace: ")

# Ask the user for the directory to process
dir_to_process = "py_tests"


def change_ip():
    # Iterate over all files in the specified directory and its subdirectories
    for dirpath, dirnames, filenames in os.walk(dir_to_process):
        for filename in filenames:
            # Skip files that don't end with .expected
            if not filename.endswith('.expected'):
                continue

            # Replace the string in the file
            filepath = os.path.join(dirpath, filename)
            with fileinput.FileInput(filepath, inplace=True) as file:
                for line in file:
                    print(line.replace(string_to_replace, ip_address), end='')
            convert_to_crlf(filepath) #making sure that all files have CRLF line endings

