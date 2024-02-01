
import testscript

ip = get_ip_address()

def print_file_contents(filename):
    with open(filename, 'r') as file:
        contents = file.read()
    print(contents)