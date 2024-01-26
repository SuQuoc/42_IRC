#include "TestServer.hpp"

int	main()
{
	TestServer test_server;

	test_server.join_tests();
	test_server.client_test();
}