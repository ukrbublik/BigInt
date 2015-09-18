#include <iostream>
using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "math.h"
#include "assert.h"

/**
 * typedefs
 */
typedef int int32;
typedef short int16;
typedef char int8;
typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;
typedef unsigned char uchar;

/**
 * @class BigInt
 */
class BigInt {
public:
	// > static class members:
	static uint16 baseOutput; //base using for output (10 by default)
private:
	// > members:
	uint16* x;
	uint16 len;
	bool neg;

public:
	// > ctors:
	BigInt();
	BigInt(const BigInt& b); //copy ctor
	BigInt(const char* str, uint16 base = 0);
	BigInt(uint16 n);
	BigInt(uint32 n);
	BigInt(int16 n);
	BigInt(int32 n);
	
	// > destructor:
	~BigInt();
	
	// > getters, setters:
	bool isNegative();
	void setNegative(bool _neg);
	
	// > methods:
	void copy(const BigInt& b); //copy
	void no_zeros(); //сократить нули
	// сравнение:
private:
	static int cmp(const uint16* a, uint16 a_len, const uint16* b, uint16 b_len);
public:
	static int cmp_abs(const BigInt& a, const BigInt& b);
	static int cmp_sign(const BigInt& a, const BigInt& b);
	bool is_zero() const;
	// арифм. операции + - * / %
	static BigInt add(const BigInt& a, const BigInt& b, bool sub_xor = false);
	static BigInt sub(const BigInt& a_, const BigInt& b_, bool sum_xor = false);
	void sub(const BigInt& b);
	static BigInt mul(const BigInt& a, uint16 b);
	static BigInt mul(const BigInt& a, const BigInt& b);
	static BigInt div(const BigInt& a, uint16 b, uint16* rest_out);
	static uint16 mod(const BigInt& a, uint16 b);
	static BigInt div(const BigInt& a, const BigInt& b, BigInt** rest_out, bool mod = false);
	static BigInt mod(const BigInt& a, const BigInt& b);
	// from/to string
	void fromString(const char* str, uint16 base = 0);
	char* toString(char** strToDelete = NULL, uint16 base = 0, bool with_prefix = true, bool with_sign = true, bool big = true) const;
	
	// > operators:
	BigInt& operator=(const BigInt& b); //операция присваивания - копированием
	friend BigInt operator+(const BigInt& a, const BigInt& b);
	friend BigInt operator-(const BigInt& a, const BigInt& b);
	friend BigInt operator*(const BigInt& a, const BigInt& b);
	friend BigInt operator/(const BigInt& a, const BigInt& b);
	friend BigInt operator%(const BigInt& a, const BigInt& b);
	friend bool operator==(const BigInt& a, const BigInt& b);
	friend bool operator!=(const BigInt& a, const BigInt& b);
	friend bool operator>(const BigInt& a, const BigInt& b);
	friend bool operator<(const BigInt& a, const BigInt& b);
	friend bool operator>=(const BigInt& a, const BigInt& b);
	friend bool operator<=(const BigInt& a, const BigInt& b);
	friend istream& operator>>(istream& _in, BigInt& a);
	friend ostream& operator<<(ostream& _out, const BigInt& a);
};
