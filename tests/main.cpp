#include "TestServer.hpp"

int	main()
{
	TestServer test_server;

	test_server.client_tests(); //doesn't return to main!?
	test_server.join_tests();
	test_server.part_tests();
	test_server.registration_tests();
	test_server.oper_tests();
	test_server.quit_tests();
}