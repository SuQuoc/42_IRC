#include "TestServer.hpp"

int	main()
{
	TestServer test_server;

	test_server.join_tests();
	test_server.part_tests();
	test_server.kick_tests();
	test_server.client_test();//doesn't return to main!?
}