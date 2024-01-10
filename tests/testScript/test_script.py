# test_script.py

import hexchat
import time

# Change these values accordingly
server = "irc.example.com"
port = 6667
channels = ["#channel1", "#channel2"]
nicknames = ["user1", "user2", "user3"]  # Add more nicknames as needed

def connect_to_server():
    hexchat.command(f"/server {server} {port}")
    hexchat.hook_server("376", on_connect)

def on_connect(word, word_eol, userdata):
    for channel in channels:
        hexchat.command(f"/join {channel}")

def change_nickname():
    for nickname in nicknames:
        hexchat.command(f"/nick {nickname}")
        time.sleep(1)  # Adjust the delay as needed
        # Add any additional commands or actions here

def on_quit(word, word_eol, userdata):
    hexchat.command("QUIT")
    hexchat.unhook(on_quit)

connect_to_server()
hexchat.hook_server("ERROR", on_quit)
hexchat.hook_server("433", lambda *args: change_nickname())

hexchat.prnt("Testing script loaded. Use /test_quit to quit.")

def test_quit(word, word_eol, userdata):
    hexchat.command("QUIT")
    hexchat.unload()

hexchat.hook_command("TEST_QUIT", test_quit)

"""

Explanation:

*   Edit the server, port, channels, and nicknames variables to match your IRC server and testing preferences.

*   The script connects to the server, joins the specified channels, and then changes nicknames in a loop.
    Adjust the delay (time.sleep) between nickname changes as needed.

*   You can add more commands or actions inside the loop in the change_nickname function to simulate different actions by each client.

*   The script prints a message when loaded, and you can use /test_quit to quit all instances of HexChat.

Remember to review and customize the script based on your specific testing requirements and IRC server setup. 
Be cautious when running automated scripts to avoid unnecessary disruptions on the IRC server.

"""