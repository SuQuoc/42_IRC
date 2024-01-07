
#include "irc.hpp"

#include <iostream>
#include <sstream>
#include <vector>
#include <cassert>



std::vector<std::string> splitIrcCmd(const std::string& input, const std::string& delimiter) 
{
    std::vector<std::string> result;

    size_t start = 0;
    size_t pos = input.find(delimiter);

    while (pos != std::string::npos) 
	{
        result.push_back(input.substr(start, pos - start));
        start = pos + delimiter.length();
        if (input.at(start) == ':')
        {
            result.push_back(input.substr(start));
            break;
        }
        pos = input.find(delimiter, start);
    }
    return result;
}

// Define a simple testing function
void testSplitIRCCommand(const std::string& command, const std::vector<std::string>& expected) 
{
    std::vector<std::string> result = splitIrcCmd(command, ":");
    for (const std::string& str : result) {
        std::cout << "- " << str << std::endl;
    }
    assert(result == expected);
}

bool areEqual(const std::vector<std::string>& actual, const std::vector<std::string>& expected) 
{
    if (actual.size() != expected.size()) {
        return false;
    }

    for (size_t i = 0; i < actual.size(); ++i) {
        if (actual[i] != expected[i]) {
            return false;
        }
    }

    return true;
}

void printVector(const std::vector<std::string>& strings) 
{
    for (const std::string& str : strings) {
        std::cout << "[" << str << "]" << std::endl;
    }
}


void splitIrcCmd_suite()
{
	std::vector<std::pair<std::string, std::vector<std::string>>> testCases = 
	{
        {"NICK nickname", {"NICK", "nickname"}},
        {":prefix NICK nickname", {":prefix", "NICK", "nickname"}},
        {"PRIVMSG :hello world", {":prefix", "NICK", "nickname"}},
		// {"PASS pw12\r\nNICK quoc\r\nUSER quocUSER 0 * :realname", {"PASS", "pw12\r\n", "NICK", "quoc\r\n", "quocUSER", "0", "*", ":realname"}},
        // Add more test cases as needed
    };

   // Loop to run tests and display results
    std::cout << "\n======= Running Tests =======\n" << std::endl;

    int i = 1;
    for (const auto& testCase : testCases) 
    {
        std::cout << "\n==Test[ " << i << "] =======\n" << std::endl;
        printVector(splitIrcCmd(testCase.first, " "));
        i++;
    }
    std::cout << "\n======= End of Tests =======\n" << std::endl;
}
