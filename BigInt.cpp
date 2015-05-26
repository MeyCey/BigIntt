#include "stdafx.h"
#include "BigInt.h"

//CONSTRUCTORS
BigInt::BigInt()
	:digits(1, 0), sign(Sign::positive) // initialize to one digit: zero
{

}
BigInt::BigInt(int a)
	: sign(Sign::positive)
{
	if (a < 0) {
		sign = Sign::negative;
		a *= (-1);
	}
	if (a == 0) // Handle 0 specifically since it won't be caught by the following loop
		digits.push_back(0);
	for ( ; a != 0; ) {
		int lastDigit = a % 10; // get the least sig. digit
		digits.push_back(lastDigit);
		a /= 10; // get to next digit
	}
}
BigInt::BigInt(std::string s)
	: sign(Sign::positive)
{
	if (s == "") {
		BigInt::BigInt();
		return;
	}

	if (s[0] == '-') { // handle the leading '-' sign
		sign = Sign::negative;
		s.erase(0, 1); // leave only numbers in the string
	}
	// need to read number is reverse order since that's how they are stored
	for (auto it = s.crbegin(); it != s.crend(); ++it) {
		if (isdigit(*it))
			digits.push_back((*it) - '0');
	}

}

//INTERFACE FUNCTIONS
int BigInt::toInt() const
{
	if ((*this) > std::numeric_limits<int>::max() || (*this) < std::numeric_limits<int>::min())
		throw std::runtime_error("BigInt is too big, or too small to fit in an integer");

	int result = 0;
	int currPowerOfTen = 1; // instead of using pow() function from <cmath>
	for (int i = 0; i < digits.size(); ++i) {
		result += digits[i] * currPowerOfTen;
		currPowerOfTen *= 10; // get to the next power of ten
	}

	if (sign == Sign::negative) // handle the number being negative
		result *= (-1);

	return result;
}
std::string BigInt::toString() const
{
	// requires reverse iterator of digits;
	std::string num;
	if (sign == Sign::negative)
		num += "-";
	for (auto it = digits.crbegin(); it != digits.crend(); ++it)
		num += ('0' + *it);
	return num;
}
int BigInt::digitSum() const
{
	int sum = 0;
	for (auto it = digits.cbegin(); it != digits.cend(); ++it) {
		sum += *it;
	}
	return sum;
}
BigInt BigInt::abs() const // seems very inefficient, perhaps needs some work
{
	BigInt copy = (*this);
	copy.sign = Sign::positive;
	return copy;
}

//HELPER FUNCTIONS
void BigInt::normalize()
// remove all the leading zeroes, if the whole number == 0, then leave one last
{
	for (int i = digits.size() - 1; i > 0; --i) {
		if (digits[i] != 0)
			return;
		else
			digits.pop_back();
	}
	if (digits.size() == 1 && digits[0] == 0) // make sure that zero is "positive" or else comparisons may fail
		sign = Sign::positive;
}
void BigInt::addSigDigits(int value)
//add the number as the most significant digits, eg. num is 234, addSigDigits(87) == 87234
//if zero is passed, than add one zero
{
	if (value == 0)
		digits.push_back(0);
	for (; value != 0;) {
		digits.push_back(value % 10);
		value /= 10;
	}
}
void BigInt::borrow(int toThisDig)
// Assumes, that borrowing is possible -- it is caller's responsibility to make sure it is
{
	auto thisDig = digits.begin() + toThisDig; // thisDig serves as a 'bookmark' to come back to after borrowing
	auto it = thisDig + 1; // it is the actual "borrowing" iterator
	for (; (*it) != 0; ++it) {
		// NOTHING - loop gets 'it' to first non-zero digit after thisDig
	}
	(*it) -= 1; // the actual borrowing
	for (; it != thisDig; --it) // If the number borrowed from is not next to thisDigit, then need to add 9 to the zeros in between
		(*it) += 9;
	(*it) += 10; // finally got to the borrowed digit; increase it by 10 (because there will be no further borrowing)
}

//ARITHMETIC OPERATORS
BigInt& BigInt::operator+=(const BigInt& rhs) // Implements the basic "long addition"
{
	if (sign != rhs.sign) { // it really is a subtraction -- (a + (-b) == a - b) && ( (-a) + b == b - a )
		*this -= ((-1) * rhs);
		return *this;
	}
	if (rhs.size() > this->size()) //if a is bigger than expand the space to fit all the digits
		digits.resize(rhs.size());
	int carryOver = 0;
	for (int i = 0; i < rhs.size(); ++i) {
		int sum = carryOver + digits[i] + rhs.digits[i];
		digits[i] = sum % 10; // set to the last digit of sum: eg 5 + 7 = 12, so you set digit[i] to 2
		carryOver = sum / 10; // carry over the rest of the sum: in case of 12 it is 12/10 == 1 (truncated)
	}
	// Take care of the leftover carryOver

	if (carryOver != 0) {
		if (rhs.size() < this->size()) { // *this has got place to fit some carry over from rhs -- no need to expand the vector of digits
			for (int i = rhs.size(); (i < this->size()) && (carryOver != 0); ++i) {
				int sum = digits[i] + carryOver;
				digits[i] = sum % 10;
				carryOver = sum / 10;
			}
		}
		if (carryOver != 0) // carry over is still nonzero; need to expand vector of digits
			addSigDigits(carryOver);
	}
	normalize();
	return *this;
}
BigInt operator+(const BigInt& lhs, const BigInt& rhs) 
// simply use the operator+=
{
	BigInt res{ lhs };
	res += rhs;
	return res;
}

BigInt& BigInt::operator-=(const BigInt& rhs) // Implements the basic "long subratction"
{
	if (sign != rhs.sign) { // it really is sum (a - (-b) == a + b && (-a) - b == -(a + b)
		*this += ((-1) * rhs);
		return *this;
	}

	if ((this->abs()) < rhs.abs()) { // make sure, that long subraction is possible - if |*this| < |rhs| than borrowing will not be possible at some point
		BigInt copyRhs{ rhs };
		copyRhs -= (*this);
		(*this) = copyRhs;
		if (sign == Sign::positive) // a - b == -(b - a)
			sign = Sign::negative;
		else
			sign = Sign::positive; // (-a) - (-b) == (-a) + b == b - a
		return *this;
	}

	for (int i = 0; i < rhs.size(); ++i) { // subtract corresponding digits; if second one is bigger, then borrow
		if (digits[i] < rhs.digits[i])
			borrow(i);
		digits[i] -= rhs.digits[i];
	}
	
	normalize(); // delete leading zeros
	return (*this);
}
BigInt operator-(const BigInt& lhs, const BigInt& rhs)
// simply use the operator-=
{
	BigInt res{ lhs };
	res -= rhs;
	return res;
}

BigInt&  BigInt::operator*=(const BigInt& rhs) // Implements the basic "long multiplication"
{
	if (sign != rhs.sign) // If signs are different than result is negative
		sign = Sign::negative;
	else // If they are the same, than the result is positive
		sign = Sign::positive;
		
	BigInt result;
	for (int i = 0; i < rhs.size(); ++i) {
		// when multiplying you have multiplying number by each digit of the other, and then summing the results
		BigInt midSum;
		for (int j = 0; j < (i - 1); ++j) {
			// when multiplying every next sum starts at 10x bigger than previous
			// BigInt() initializes to digits to vector<int>(1, 0) so no need to change the value untill you need two zeros
			midSum.addSigDigits(0);
		}
		int carryOver = 0;
		for (int j = 0; j < this->size(); ++j) {
			int result = (rhs.digits[i] * digits[j]) + carryOver;
			if ((i == 0) && (j == 0)) // BigInt() initializes digits to vector<int>(1, 0)
				midSum.digits[0] = result % 10;
			else
				midSum.addSigDigits((result % 10)); // last digit, in the proper power
			carryOver = result / 10;
		}
		// take care of leftover carryOver
		if (carryOver != 0) 
				midSum.addSigDigits(carryOver);
		// add the created midSum to the overall result of the multiplication
		result += midSum;
	}
	result.normalize();
	digits = result.digits; // not assigning fully (*this = result) so as to preserve sign information
	return *this;

}
BigInt operator*(const BigInt& lhs, const BigInt& rhs)
{
	// simply use operator*=
	BigInt result{ lhs };
	result *= rhs;
	return result;
}

//OTHER MATHEMATICAL FUNCTIONS
void BigInt::pow(int n)
{
	// NOT IMPLEMENTED
	std::cout << "POW NOT IMPLEMENTED!\n";
}

//COMPARISON OPERATORS
bool operator==(const BigInt& lhs, const BigInt& rhs)
{
	// different signs == different numbers
	if ((lhs.size() != rhs.size()) || (lhs.sign != rhs.sign))
		return false;
	else
		for (int i = lhs.size() - 1; i >= 0; --i) {
			if (lhs.digits[i] != rhs.digits[i])
				return false;
		}
	return true; // sign and all digits are the same - numbers are equal
}
bool operator!=(const BigInt& lhs, const BigInt& rhs)
{
	return (!(lhs == rhs));
}
bool operator<(const BigInt& lhs, const BigInt& rhs)
{
	// if signs are not the same, than the smaller number is the one with negative sign
	if (lhs.sign != rhs.sign)
		return (!(lhs.sign == BigInt::Sign::positive));

	// if they are positive, then small absolute value == smaller number
	if (lhs.sign == BigInt::Sign::positive) {
		if (lhs.size() != rhs.size())
			return (lhs.size() < rhs.size());
		for (int i = lhs.size() - 1; i >= 0; --i) {
			if (lhs.digits[i] != rhs.digits[i])
				return (lhs.digits[i] < rhs.digits[i]);
		}
	}

	// and with negatives the situation is reversed
	if (lhs.sign == BigInt::Sign::negative) {
		if (lhs.size() != rhs.size())
			return (!(lhs.size() < rhs.size()));
		for (int i = lhs.size() - 1; i >= 0; --i) {
			if (lhs.digits[i] != rhs.digits[i])
				return (!(lhs.digits[i] < rhs.digits[i]));
		}
	}
	return false; // numbers are equal; return false
}
bool operator>(const BigInt& lhs, const BigInt& rhs)
{
	// essentialy copy-pasted operator<, with some changes of '<' to '>', etc
	if (lhs.sign != rhs.sign)
		return (lhs.sign == BigInt::Sign::positive);

	if (lhs.sign == BigInt::Sign::positive) {
		if (lhs.size() != rhs.size())
			return (lhs.size() > rhs.size());
		for (int i = lhs.size() - 1; i >= 0; --i) {
			if (lhs.digits[i] != rhs.digits[i])
				return (lhs.digits[i] > rhs.digits[i]);
		}
	}

	if (lhs.sign == BigInt::Sign::negative) {
		if (lhs.size() != rhs.size())
			return (!(lhs.size() > rhs.size()));
		for (int i = lhs.size() - 1; i >= 0; --i) {
			if (lhs.digits[i] != rhs.digits[i])
				return (!(lhs.digits[i] > rhs.digits[i]));
		}
	}

	return false; // numbers are equal; return false
}
bool operator<=(const BigInt& lhs, const BigInt& rhs)
{
	return (!(rhs > lhs));

	/* I'll just... keep it here
	if (lhs.sign != rhs.sign)
		return (!(lhs.sign == BigInt::Sign::positive));
	if (lhs.sign == BigInt::Sign::positive) {
		if (lhs.size() != rhs.size())
			return (lhs.size() < rhs.size());
		for (int i = lhs.size() - 1; i >= 0; --i) {
			if (lhs.digits[i] != rhs.digits[i])
				return (lhs.digits[i] < rhs.digits[i]);
		}
	}
	if (lhs.sign == BigInt::Sign::negative) {
		if (lhs.size() != rhs.size())
			return (!(lhs.size() < rhs.size()));
		for (int i = lhs.size() - 1; i >= 0; --i) {
			if (lhs.digits[i] != rhs.digits[i])
				return (!(lhs.digits[i] < rhs.digits[i]));
		}
	}
	return true; // numbers are equal; return true*/
}
bool operator>=(const BigInt& lhs, const BigInt& rhs)
{
	return (!(lhs < rhs));

	/* Seriously... sigh
	if (lhs.sign != rhs.sign)
		return (lhs.sign == BigInt::Sign::positive);
	if (lhs.sign == BigInt::Sign::positive) {
		if (lhs.size() != rhs.size())
			return (lhs.size() > rhs.size());
		for (int i = lhs.size() - 1; i >= 0; --i) {
			if (lhs.digits[i] != rhs.digits[i])
				return (lhs.digits[i] > rhs.digits[i]);
		}
	}
	if (lhs.sign == BigInt::Sign::negative) {
		if (lhs.size() != rhs.size())
			return (!(lhs.size() > rhs.size()));
		for (int i = lhs.size() - 1; i >= 0; --i) {
			if (lhs.digits[i] != rhs.digits[i])
				return (!(lhs.digits[i] > rhs.digits[i]));
		}
	}
	return true; // numbers are equal; return true*/
}

//FREE FUNCTIONS
std::ostream& operator<<(std::ostream& os, const BigInt& bi)
{
	os << bi.toString(); // use the member function of BigInt
	return os;
}