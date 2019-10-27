#include <iostream>
#include "Log.h"

void LogInit()
{
	Log("Log Init");
}

void Log(const char* message)
{
	std::cout << message << std::endl;
}