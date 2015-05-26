// main.cpp : Defines the entry point for the console application.
//

#include <fstream>
#include <iostream>
#include "BigInt.h"
#include "TestBigInt.h"

int main()
{
	std::ifstream ifs{ "BigIntTestCases.txt" };
	std::vector<TestBigInt> tests;

	for (;;) {
		TestBigInt t;
		if (!(ifs >> t)) break;
		tests.push_back(t);
	}
	std::ofstream ofs{ "TestOutput.txt" };
	for (auto it = tests.begin(); it != tests.end(); ++it) {
		std::cout << "Testing: " << it->label << ' ' << it->num1 << ' ' << it->num2 << ' ' << it->autoChecked << '\n'; 
		performTest(ofs, (*it));
	}
	std::cout << "Tests Complete! \n";

	return 0;
}

