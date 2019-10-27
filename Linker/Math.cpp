#include <iostream>
#include "Log.h"

int Multiply(int a, int b)
{
	Log("Multyply");
	return a * b;
}

int main()
{
	std::cout << Multiply(5, 8) << std::endl;
	std::cin.get();
}