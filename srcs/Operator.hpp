
# pragma once

#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

#include "User.hpp"

class Operator: public User
{
private:

	void kick() const;
	void invite() const;
	void topic() const;
	void mode() const;
	
public:


};
