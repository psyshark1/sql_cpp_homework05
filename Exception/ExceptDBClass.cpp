#include"ExceptDBClass.h"

std::string ExceptDBClass::what()
{
	return message;
}

ExceptDBClass::ExceptDBClass(std::string&& msg) : message{ std::move(msg) }
{
}

ExceptDBClass::~ExceptDBClass()
{
}