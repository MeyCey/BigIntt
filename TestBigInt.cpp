#include "TestBigInt.h"

void TestBigInt::doTests()
// do the BigInt operators tests; fill the vector outResult
{
	BigInt firstNum{ num1 };
	BigInt secNum{ num2 };

	outResult.push_back((firstNum + secNum).toString());
	outResult.push_back((secNum + firstNum).toString());
	outResult.push_back((firstNum - secNum).toString());
	outResult.push_back((secNum - firstNum).toString());
	outResult.push_back((firstNum * secNum).toString());
	outResult.push_back((secNum * firstNum).toString());
	std::cout << "pushed_back outResults\n"; // for debugging purposes
}

void TestBigInt::autoFillExR()
// fill the expected results vector
{
	int firstNum = stringToI(num1);
	int secNum = stringToI(num2);

	exResult.push_back(iToString(firstNum + secNum));
	exResult.push_back(iToString(secNum + firstNum));
	exResult.push_back(iToString(firstNum - secNum));
	exResult.push_back(iToString(secNum - firstNum));
	exResult.push_back(iToString(firstNum * secNum));
	exResult.push_back(iToString(secNum * firstNum));
	std::cout << "pushed_back exResults\n"; // put it here for debugging purposes
}

std::istream& operator>>(std::istream& is, TestBigInt& t)
/* Read input in the form:
*	{ label num1 num2 autoChecked{ result+ result- result* } } -- if (autoChecked == false)
*	{ label num1 num2 autoChecked{ } } -- if (autoChecked == true)
*/
{
	char ch1;
	is >> ch1;
	if (ch1 != '{') { // no tests anymore
		is.unget();
		is.clear(std::ios_base::failbit);
		return is;
	}

	char ch2;
	std::string lab;
	std::string firstNum;
	std::string secNum;
	bool check;
	is >> lab >> firstNum >> secNum >> check >> ch2;
	if ((ch2 != '{'))
		throw std::runtime_error("invalid formatting brace");

	t.label = lab;
	t.num1 = firstNum;
	t.num2 = secNum;
	t.autoChecked = check;

	if (!(check)) { // not auto-checking; need to input expected results
		for (int i = 0; i < t.amountOfResults; ++i) {
			std::string s;
			is >> s;
			if (s == "}")
				throw std::runtime_error("invalid number of expected results");
			t.exResult.push_back(s);
		}
	}

	char ch3, ch4;
	is >> ch3 >> ch4;
	if ((ch3 != '}') || (ch4 != '}'))
		throw std::runtime_error("invalid formatting brace");
	
	return is;
}

std::ostream& performTest(std::ostream& os, TestBigInt& t)
// Test the BigInt with the given test case; if unexpected result happens than write it to os
{	
	// if the results are to be autochecked, than fill the exResult -- else assume it is already full
	if (t.autoChecked)
		t.autoFillExR();

	t.doTests(); // do the given tests with BigInt

	// the condition part is verbose, since it is assumed that vectors are of exact same size anyway
	for (auto itE = t.exResult.cbegin(), itO = t.outResult.cbegin(); (itE != t.exResult.cend()) || (itO != t.outResult.cend()); ++itE, ++itO)
		if (*itE != *itO)
			os << "Test not passed: " << t.label  << ", " << distance(t.exResult.cbegin(), itE) << "\n\tExpected: " << *itE << ", got " << *itO << "\n";
	
	return os;
}

int stringToI(const std::string& s)
// simple string-to-integer conversion
{
	std::istringstream is{ s };
	int res;
	is >> res;
	return res;
}

std::string iToString(int a)
// simple integer-to-string conversion
{
	std::ostringstream os;
	os << a;
	return (os.str());
}