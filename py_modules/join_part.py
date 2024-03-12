import os
from .utils import *

#------------------JOIN------------------
def joiningTooManyChannels(server):
	test_name = "joiningTooManyChannels"
	vector = []
	for i in range(11):
		join_msg = "JOIN #chan" + str(i)
		vector.append((0, join_msg))		
	runMultiClientTest(test_name, 1, vector, server)

def fullChannel(server):
	test_name = "fullChannel"
	vector = [
		(0, "JOIN #chan"),
		(1, "JOIN #chan"),
		(2, "JOIN #chan"),
		(3, "JOIN #chan"),
		(4, "JOIN #chan"),
		(5, "JOIN #chan"),
		(6, "JOIN #chan"),
		(7, "JOIN #chan"),
		(8, "JOIN #chan"),
		(9, "JOIN #chan"), #channel is full - 10 clients
		(10, "JOIN #chan"), #trying to join a full channel
	]
	runMultiClientTest(test_name, 11, vector, server)

def joinTooManyChannelsWithList(server):
	test_name = "joinTooManyChannelsWithList"
	vector = [
		(0, "JOIN #chan0,#chan1,#chan2,#chan3,#chan4,#chan5,#chan6,#chan7,#chan8,#chan9,#chan10"), #joining 11 channels
		(0, "JOIN #chan10,#chan11,"), #only joined 10 before due to list limit, now error should be triggered
	]
	runMultiClientTest(test_name, 1, vector, server)

def inviteOnlyChannel(server):
	test_name = "inviteOnlyChannel"
	vector = [
		(0, "JOINT #chan"),
		# (0, "MODE #chan +i"),
		# (1, "JOIN #chan"), #cant join
		# (0, "INVITE client1 #chan"),
		# (1, "JOIN #chan"), #can join now
		]
	runMultiClientTest(test_name, 2, vector, server)

def inviteOnlyChannelOrder(server):
	test_name = "inviteOnlyChannelOrder"
	vector = [
		(0, "JOIN #chan"),
		(0, "INVITE client1 #chan"),
		(0, "MODE #chan +i"),
		(1, "JOIN #chan"), #can join now
		]
	runMultiClientTest(test_name, 2, vector, server)
	
def badChannelKey(server):
	test_name = "badChannelKey"
	vector = [
		(0, "JOIN #chan"),
		(0, "MODE #chan +k Key"),
		(1, "JOIN #chan"), #cant join
		(1, "JOIN #chan wrong key"), #cant join
		(1, "JOIN #chan key"), #cant join case sensitive?
		(1, "JOIN #chan Key"), #cant join case sensitive?
		]
	runMultiClientTest(test_name, 2, vector, server)

def testJOIN(server, original_directory):
	setupTest("join")
	#errNeedMoreParams("JOIN", server)
	#errNOSUCHCHANNEL("JOIN #nonexistentchannel", server)
	
	joiningTooManyChannels(server) #1 client trying to join 11 channels
	joinTooManyChannelsWithList(server) #1 client trying to join 11 channels in a list
	fullChannel(server) #11 clients trying to join  
	inviteOnlyChannel(server)
	inviteOnlyChannelOrder(server) #weird order
	badChannelKey(server)
	os.chdir(original_directory)

#------------------PART------------------
def partingChannel(server):
	test_name = "partingChannel"
	vector = [
		(0, "JOIN #chan"),
		(1, "JOIN #chan"),
		(0, "PART #chan"),
		(0, "PART #chan"),
		(1, "MODE #chan +i"),
		]
	runMultiClientTest(test_name, 2, vector, server)

def testPART(server, original_directory):
	setupTest("part")
	#errNeedMoreParams("PART", server)
	#errNOSUCHCHANNEL("PART #nonexistentchannel", server)
	partingChannel(server)
	os.chdir(original_directory)