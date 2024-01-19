#include <iostream>
#include <string>

std::string getName() {
    // Replace this with your actual implementation of getName()
    return "John";
}

int main() {
    std::string error_message = "Error: ";
    error_message += getName() + " <nickname> :No such nick/channel";

    // Display the result
    std::cout << error_message << std::endl;

    return 0;
}
