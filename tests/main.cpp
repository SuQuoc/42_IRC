#include "TestServer.hpp"

int	main()
{
	TestServer test_server;

	test_server.client_tests();
	test_server.join_tests();
	test_server.part_tests();
	test_server.registration_tests();
	test_server.oper_tests();
	test_server.quit_tests();
	test_server.kill_tests();
	test_server.kick_tests();
	test_server.client_test();//doesn't return to main!?
}