import os
from .utils import *

#------------------OPER + KILL------------------
def wrongOperHost(server):
	test_name = "wrongOperHost"
	vector = [(0, "OPER 7.7.7.7 wrongOperHost")]
	runMultiClientTest(test_name, 1, vector, server)

def wrongOperPw(server):
	test_name = "wrongOperPw"

	vector = [(0, "OPER " + get_ip_address() + " wrongOperPw")]
	runMultiClientTest(test_name, 1, vector, server)

def kill(server):
	test_name = "oper_kill"
	
	vector = [
		(0, "KILL client0 :cant do i have no priviliges"), #ERR_NOPRIVILEGES
		(0, "OPER " + get_ip_address() + " operPW"), #--> You are now an IRC operator
		(0, "KILL"), #ERR_NEEDMOREPARAMS
		(0, "KILL NoNick "), #ERR_NOSUCHNICK
		(0, "PRIVMSG client1 :you should see this cuz client 1 is alive and was not kiled"),
		(1, "PRIVMSG client0 :yes i am alive"),
		(0, "KILL client1 :successfull kill of client1"),
		(0, "PRIVMSG client1 :'No such nick/channel' should be sent"),
		(0, "KILL client0 :killing myself??"),
		(2, "PRIVMSG client0 :'No such nick/channel' should be sent"),
	]
	runMultiClientTest(test_name, 3, vector, server)

def deleteChannelwithKill(server):
	test_name = "deleteChannelwithKill"
	vector = [
		(1, "JOIN #chan"),
		(0, "TOPIC #chan"),
		(0, "OPER " + get_ip_address() + " operPW"),
		(0, "KILL client1 :kill client1"),
		(0, "TOPIC #chan"),
	]
	runMultiClientTest(test_name, 2, vector, server)

def testOPER(server, original_directory):
	setupTest("oper")
	errNeedMoreParams("OPER", server)
	wrongOperHost(server)
	wrongOperPw(server)
	kill(server)
	deleteChannelwithKill(server)
	os.chdir(original_directory)
