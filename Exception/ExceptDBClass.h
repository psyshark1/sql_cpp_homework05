#pragma once
#include<stdexcept>

const class ExceptDBClass
{
public:
	ExceptDBClass(std::string&& msg);
	~ExceptDBClass();
	std::string what();
private:
	const std::string message;
};