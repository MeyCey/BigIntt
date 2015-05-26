#pragma once
/** Very simple BigInt library. Stores numbers as vector<int>,
* where each element of the vector is one digit of the number.
* Digits are sorted "in reverse order": digits[0] refers to least
* significant digit of the number. It also happens that it is
* the exponent to which the base is raised:
* if digit[3] == n, then the value of n == n * 10^3
*/

/*
* TO IMPLEMENT:
*   [ ] arithmetic operators
*   [x] normalize function
*   [x] comparing operators
*   [ ] increment(decrement) operators
*   [ ] constructors:
*		[ ] all integer types
*		[x] string
*		[ ] (char*)?
*		[ ] BigInt -- the default should work, no need to write my own for now
*	[ ] conversions (to int, double, etc.)
*	[ ] binary, hex, (octal?) conversion
*   [ ] input operators (also in binary, hex, [octal?] form)
*   [x] digit sum
*	[ ] factorisation
*/

#include <iostream>
#include <limits>
#include <string>
#include <vector>

class BigInt
{
public:
	//CONSTRUCTORS
	BigInt(); // intialize to 0
	BigInt(int a); // int initializer
	BigInt(std::string s); // string initializer

	//INTERFACE FUNCTIONS
	int toInt() const; // returns int form of the number, if it can fit; else throws exception
	std::string toString() const; // returns string form of the number
	int size() const { return digits.size(); } // returns number of digits
	int digitSum() const; // returns the sum of the digits within the number, USES INT, NOT BIGINT
	BigInt abs() const; // return absolute value of the number, seems kinda inefficient

	//OUTPUT & INPUT OPERATORS
	friend std::ostream& operator<<(std::ostream& os, const BigInt& bi); // this one currently does not need friend status
	friend std::istream& operator>>(std::istream& is, const BigInt& a); // NOT IMPLEMENTED

	//ARITHMETIC OPERATORS
	BigInt& operator+=(const BigInt& rhs); // Implements the basic "long addition"
	friend BigInt operator+(const BigInt& lhs, const BigInt& rhs);

	BigInt& operator-=(const BigInt& rhs); // Implements the basic "long subtraction"
	friend BigInt operator-(const BigInt& lhs, const BigInt& rhs);

	BigInt& operator*=(const BigInt& rhs); // Implements the basic "long multiplication"
	friend BigInt operator*(const BigInt& lhs, const BigInt& rhs);

	BigInt& operator/=(const BigInt& a); // NOT IMPLEMENTED
	friend BigInt operator/(const BigInt& a, const BigInt& b);

	BigInt& operator%=(const BigInt& lhs); // NOT IMPLEMENTED
	friend BigInt operator%(const BigInt& lhs, const BigInt& rhs);

	BigInt operator+(); // NOT IMPLEMENTED; Unary plus: does nothing
	BigInt operator-(); // NOT IMPLEMENTED; Unary minus: reverse the sign

	//OTHER MATHEMATICAL FUNCTIONS
	void pow(int n); // NOT IMPLEMENTED; raise (*this) to the power n

	//COMPARISON OPERATORS
	friend bool operator==(const BigInt& lhs, const BigInt& rhs);
	friend bool operator!=(const BigInt& lhs, const BigInt& rhs);
	
	friend bool operator<(const BigInt& lhs, const BigInt& rhs);
	friend bool operator>(const BigInt& lhs, const BigInt& rhs);

	friend bool operator<=(const BigInt& lhs, const BigInt& rhs);
	friend bool operator>=(const BigInt& lhs, const BigInt& rhs);
private:
	//HELPER FUNCTIONS
	void normalize(); // remove all leading zeros, if all of digits == 0, then leave digits[0]
	void addSigDigits(int value); // add the number as the most significant digits (value > 0), eg. num is 234, addSigDigits(87) == 87234
	void borrow(int toThisDig); // borrow to this digit (used for subtraction)

	//THE NUMBER, AND SIGN STORED
	std::vector<int> digits;
	enum class Sign { positive, negative };
	Sign sign; // Positive == 0(false), Negative == 1(true)

};

//FREE FUNCTIONS
std::ostream& operator<<(std::ostream& os, const BigInt& bi);
std::istream& operator>>(std::istream& is, const BigInt& bi); // NOT IMPLEMENTED

BigInt operator+(const BigInt& lhs, const BigInt& rhs);
BigInt operator-(const BigInt& lhs, const BigInt& rhs);
BigInt operator*(const BigInt& lhs, const BigInt& rhs);
BigInt operator/(const BigInt& a, const BigInt& b); // NOT IMPLEMENTED
BigInt operator%(const BigInt& lhs, const BigInt& rhs); // NOT IMPLEMENTED

bool operator==(const BigInt& lhs, const BigInt& rhs);
bool operator!=(const BigInt& lhs, const BigInt& rhs);
bool operator<(const BigInt& lhs, const BigInt& rhs);
bool operator>(const BigInt& lhs, const BigInt& rhs);
bool operator<=(const BigInt& lhs, const BigInt& rhs);
bool operator>=(const BigInt& lhs, const BigInt& rhs);