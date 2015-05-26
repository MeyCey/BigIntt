#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include "BigInt.h"

/* Class used for testing my own BigInt library
* Stores tested numbers as string,
* because some tests are on numbers bigger than int.
* exResult vector is empty when test is auto-checked,
* which happens when num1 and num2 aren't bigger than int,
* so you can leave calculating the expected result to
* normal int arithmetic.
* Else you need to input exactly the required number of
* expected results into the vector exResult
* The format of the text for the operator>> is:
*	{ label num1 num2 autoChecked{ result+ result- result* } }
*	{ label num1 num2 autoChecked{ } }
* There have to be all operators' results given, or none
* The order of tests:
*	num1 + num2
*	num2 + num1
*	num1 - num2
*	num2 - num1
*	num1 * num2
*	num2 * num1
*/
struct TestBigInt
{
	void doTests(); // apply the operators (with BigInts); fills outResult
	void autoFillExR(); // fills exResult for autochecking (i.e. convert to int, do calculations, and than back to string)

	static constexpr int amountOfResults = 6; // exactly this many results have to be read to exResult
	std::string label; // test case label
	std::string num1; // numbers to be checked
	std::string num2;
	bool autoChecked; // true if TestBigInt is to calculate expected results by itself

	std::vector<std::string> exResult; // expected results
	std::vector<std::string> outResult; // results output by BigInt calculations
};

std::istream& operator>>(std::istream& is, TestBigInt& t);

// Test the BigInt with the given test case; if unexpected result happens than write it to os
std::ostream& performTest(std::ostream& os, TestBigInt& t);

// Needed for auto-checking
int stringToI(const std::string& s);
std::string iToString(int a);