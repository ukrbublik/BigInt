#include <iostream>
using namespace std;
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "math.h"
#include "assert.h"

//typedefs
typedef int int32;
typedef short int16;
typedef char int8;
typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;
typedef unsigned char uchar;



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

uint16 BigInt::baseOutput = 10; //default base for output


/*
 * ctors
 */
BigInt::BigInt()
{
	this->x = NULL;
	this->len = 0;
	this->neg = false;
}

//copy
void BigInt::copy(const BigInt& b)
{
	delete this->x; //delete old
	
	this->neg = b.neg;
	this->len = b.len;
	this->x = new uint16[b.len];
	for(int i = 0 ; i < b.len ; i++)
		this->x[i] = b.x[i];
}

//copy ctor
BigInt::BigInt(const BigInt& b)
{
	this->neg = b.neg;
	this->len = b.len;
	this->x = new uint16[b.len];
	for(int i = 0 ; i < b.len ; i++)
		this->x[i] = b.x[i];
}
	
BigInt::BigInt(const char* str, uint16 base/* = 0*/)
{
	BigInt();
	this->fromString(str, base);
}

BigInt::BigInt(uint16 n) {
	this->neg = false;
	this->len = 1;
	this->x = new uint16[this->len];
	this->x[0] = n;
	this->no_zeros();
}
BigInt::BigInt(uint32 n) {
	this->neg = false;
	this->len = 2;
	this->x = new uint16[this->len];
	this->x[0] = (uint16)(n & 0x0000FFFF);
	this->x[1] = (uint16)((n >> 16) & 0x0000FFFF);
	this->no_zeros();
}
BigInt::BigInt(int16 n) {
	this->neg = (n < 0);
	n = (n > 0 ? n : n*(-1));
	this->len = 1;
	this->x = new uint16[this->len];
	this->x[0] = (uint16)n;
	this->no_zeros();
}
BigInt::BigInt(int32 n) {
	this->neg = (n < 0);
	n = (n > 0 ? n : n*(-1));
	this->len = 2;
	this->x = new uint16[this->len];
	this->x[0] = (uint16)(n & 0x0000FFFF);
	this->x[1] = (uint16)((n >> 16) & 0x0000FFFF);
	this->no_zeros();
}

/*
 * ~
 */
BigInt::~BigInt()
{
	delete x;
}

/*
 * neg: get(), set()
 */
bool BigInt::isNegative()
{
	return this->neg;
}
void BigInt::setNegative(bool _neg)
{
	this->neg = _neg;
}

/*
 * base compare function, for arrays of uint16
 */
int BigInt::cmp(const uint16* a, uint16 a_len, const uint16* b, uint16 b_len)
{
	for(int i = (a_len >= b_len ? a_len : b_len) - 1 ; i >= 0 ; i--) {
		if((i < a_len ? a[i] : 0) != (i < b_len ? b[i] : 0)) {
			return ((i < a_len ? a[i] : 0) > (i < b_len ? b[i] : 0)) ? 1 : -1;
		}
	}
	return 0;
}

/*
 * compare |a| <=> |b|
 */
int BigInt::cmp_abs(const BigInt& a, const BigInt& b)
{
	return BigInt::cmp(a.x, a.len, b.x, b.len);
}

/*
 * compare a <=> b, sign matters!
 */
int BigInt::cmp_sign(const BigInt& a, const BigInt& b)
{
	if(a.neg ^ b.neg)
		return (a.neg ? -1 : 1);
	else
		return BigInt::cmp(a.x, a.len, b.x, b.len) * (a.neg ? -1 : 1);
}

/*
 * == 0
 */
bool BigInt::is_zero() const
{
	for(int i = 0 ; i < this->len ; i++)
		if(this->x[i])
			return false;
	return true;
}

/*
 * сократить нули
 */
void BigInt::no_zeros()
{
	for(int i = this->len - 1 ; i >= 0 ; i--) {
		if(this->x[i])
			break;
		else
			this->len--;
	}
}

/*
 * a + b
 *
 * sub_xor - если вызов сделан из sub() для a и b разных знаков: a - (-b) = a + b  или  -a - b = - (a + b)
 */
BigInt BigInt::add(const BigInt& a, const BigInt& b, bool sub_xor/* = false*/)
{
	if(!sub_xor && (a.neg ^ b.neg)) // a + (-b) or -a + b
		return BigInt::sub(a, b, true);

	BigInt c;
	c.neg = a.neg; //для случая sub_xor надо брать знак a, для случая сложения знаки a и b равны
	c.len = max(a.len, b.len) + 1;
	c.x = new uint16[c.len];
	memset(c.x, 0, c.len * 2);
	for(int i = 0 ; i < (c.len - 1) ; i++) {
		*((uint32*)(c.x + i)) = (uint32)0 + (i < a.len ? a.x[i] : 0) + (i < b.len ? b.x[i] : 0) + c.x[i];  //write result in LE-memory
	}
	
	c.no_zeros(); //сократим нули
	
	return c;
}

/*
 * a - b
 *
 * sum_xor - если вызов сделан из sum() для a и b разных знаков: a + (-b) = a - b  или  -a + b = b - a
 */
BigInt BigInt::sub(const BigInt& a_, const BigInt& b_, bool sum_xor/* = false*/)
{
	if(!sum_xor && (a_.neg ^ b_.neg)) { //a-(-b) or (-a)-b
		return BigInt::add(a_, b_, true);
	}
	
	bool switch_b_a = false, neg_result = false;
	if(!sum_xor && (a_.neg && b_.neg)) { //-a-(-b) = b-a
		switch_b_a = true;
	}
	if(sum_xor && (a_.neg && !b_.neg)) { //-a + b = b-a
		switch_b_a = true;
	}
	if((switch_b_a ? BigInt::cmp_abs(b_, a_) : BigInt::cmp_abs(a_, b_)) < 0) {
		switch_b_a = !switch_b_a;
		neg_result = !neg_result;
	}

	const BigInt &a = (switch_b_a ? b_ : a_), &b = (switch_b_a ? a_ : b_);
	// (a always refers to bigger number, b - to smaller)
	
	BigInt c;
	c.neg = neg_result;
	c.len = max(a.len, b.len);
	c.x = new uint16[c.len];
	memset(c.x, 0, c.len * 2);

	for(int i = 0 ; i < c.len ; i++) {
		if( (uint32)a.x[i] >= ((uint32)(i < b.len ? b.x[i] : 0) + c.x[i]) ) {
			c.x[i] = (uint32)a.x[i] - c.x[i] - (i < b.len ? b.x[i] : 0);
		} else {
			c.x[i] = (uint32)a.x[i] + 0x10000 - c.x[i] - b.x[i];
			c.x[i+1] = 1;
		}
	}
	
	c.no_zeros(); //сократим нули
	
	return c;
}

/*
 * a - b (result to a)
 */
void BigInt::sub(const BigInt& b)
{
	assert(!(this->neg ^ b.neg)); // not a-(-b) or (-a)-b
	assert(BigInt::cmp_abs(*this, b) >= 0); // |a| >= |b| for a - b or -a - -b
	
	for(int i = 0 ; i < this->len ; i++) {
		if(i >= b.len) {
			continue;
		} else if(this->x[i] >= b.x[i]) {
			this->x[i] -= b.x[i];
		} else {
			//заём единицы:
			for(int j = i+1 ; j < this->len ; j++) {
				if(this->x[j]) {
					this->x[j]--;
					break;
				} else {
					this->x[j] = 0xFFFF;
				}
			}
			this->x[i] = (uint32)this->x[i] + 0x10000 - b.x[i];
		}
	}
	
	this->no_zeros(); //сократим нули
}

/*
 * a * b, b - 16 bit
 */
BigInt BigInt::mul(const BigInt& a, uint16 b)
{
	BigInt c;
	c.neg = a.neg;
	c.len = a.len + 1;
	c.x = new uint16[c.len];
	memset(c.x, 0, c.len * 2);
	
	for(int i = 0 ; i < a.len ; i++) {
		*((uint32*)(c.x + i)) += (uint32)a.x[i] * b;
	}
	
	c.no_zeros(); //сократим нули
	
	return c;
}

/*
 * a * b
 */
BigInt BigInt::mul(const BigInt& a, const BigInt& b)
{
	BigInt c;
	c.neg = (a.neg ^ b.neg);
	c.len = a.len + b.len;
	c.x = new uint16[c.len];
	memset(c.x, 0, c.len * 2);
	
	uint32 t;
	for(int i = 0 ; i < a.len ; i++) {
		for(int j = 0 ; j < b.len ; j++) {
			t = (uint32)a.x[i] * b.x[j];
			for(int k = i + j ; k < (c.len - 1) ; k++) {
				t += c.x[k];
				c.x[k] = (uint16)t; //loword
				t = ((t >> 16) & 0x0000FFFF); //hiword
			}
			c.x[c.len - 1] += (uint16)t; //loword
		}
	}
	
	c.no_zeros(); //сократим нули
	
	return c;
}

/*
 * a / b, b - 16 bit
 */
BigInt BigInt::div(const BigInt& a, uint16 b, uint16* rest_out)
{
	assert(b != 0);
	
	uint16 rest = 0;
	BigInt c;
	c.neg = a.neg;
	c.len = a.len;
	c.x = new uint16[c.len];
	memset(c.x, 0, c.len * 2);
	
	for(int i = a.len - 1 ; i >= 0 ; i--) {
		c.x[i] = (((uint32)rest << 16) + a.x[i]) / b;
		rest = (((uint32)rest << 16) + a.x[i]) % b;
	}
	
	c.no_zeros(); //сократим нули
	
	if(rest_out != NULL)
		*rest_out = rest;
	
	return c;
}

/*
 * a % b, b - 2 bytes
 */
uint16 BigInt::mod(const BigInt& a, uint16 b)
{
	assert(b != 0);
	
	uint16 rest = 0;
	for(int i = a.len - 1 ; i >= 0 ; i--) {
		rest = (((uint32)rest << 16) + a.x[i]) % b;
	}
	
	return rest;
}

/*
 * a / b
 */
BigInt BigInt::div(const BigInt& a, const BigInt& b, BigInt** rest_out, bool mod/* = false*/)
{
	assert(!b.is_zero());
	
	BigInt c;
	c.neg = (a.neg ^ b.neg);
	c.len = a.len;
	c.x = new uint16[c.len];
	memset(c.x, 0, c.len * 2);
	
	BigInt* rest_ptr;
	if(mod) {
		BigInt rest;
		rest_ptr = &rest;
	} else {
		rest_ptr = new BigInt();
	}
	if(rest_out)
		*rest_out = rest_ptr;
	
	rest_ptr->neg = a.neg;
	rest_ptr->len = b.len + 1;
	rest_ptr->x = new uint16[rest_ptr->len];
	memset(rest_ptr->x, 0, rest_ptr->len * 2);
	
	rest_ptr->len = 0; //for start
	
	for(int i = a.len - 1 ; i >= 0 ; i--) {
		// add a[i] to rest
		rest_ptr->len++;
		for(int j = rest_ptr->len - 1 ; j > 0 ; j--)
			rest_ptr->x[j] = rest_ptr->x[j - 1];
		rest_ptr->x[0] = a.x[i];
		
		// rest = rest - b * x. find max x using binary search:
		uint16 left = 0, right = 0xFFFF, center,  x = 0;
		BigInt r; // r = b * x
		while(left <= right) {
			center = ((uint32)left + right) / 2;
			r = BigInt::mul(b, center);
			if(BigInt::cmp_abs(r, *rest_ptr) <= 0) { //r подходит
				x = center;
				if(center == 0xFFFF)
					break;
				else
					left = center + 1;
			} else { //r не подходит
				if(center == 0)
					break;
				else
					right = center - 1;
			}
		}
		
		if(x != 0) {
			r = BigInt::mul(b, x);
			rest_ptr->sub(r);
		}
		rest_ptr->no_zeros(); //сократим нули
		
		c.x[i] = x;
	}
	
	c.no_zeros(); //сократим нули
	
	if(!mod && !rest_out)
		delete rest_ptr; //don't need on out, free!
	
	return (mod ? *rest_ptr : c);
}

/*
 * a % b
 */
BigInt BigInt::mod(const BigInt& a, const BigInt& b)
{
	return BigInt::div(a, b, NULL, true);
}

/*
 * from string (only 8-, 10-, 16-based)
 */
void BigInt::fromString(const char* str, uint16 base/* = 0*/)
{
	assert(base == 0 || base == 8 || base == 10 || base == 16);
	
	if(str[0] == '-') {
		this->neg = true;
		str++;
	} else {
		this->neg = false; //without '-' it's positive
	}
	
	if(str[0] == '0' && (str[1] == 'x' || str[1] == 'X')) { //0x means only 16-base
		str+=2;
		base = 16;
	} else if(base == 0 && str[0] == '0') { //0 can be for 8-base or not
		str++;
		base = 8;
	} else if(base == 0) {
		base = 10; //default
	}
	
	const char PRINTF_BASE = (base == 10 ? 'u' : (base == 16 ? 'X' : (base == 8 ? 'o' : 'u')));
	const char PRINTF_FORMAT[] = {'%', PRINTF_BASE, 0};
	char PRINTF_FORMAT_2[] = {'%', '0', '*', PRINTF_BASE, 0};
	
	delete this->x; //free old digits
	
	this->len = (uint16) ceil((double)strlen(str) * log10(base) / log10(16) / 4);
	this->x = new uint16[this->len];
	memset(this->x, 0, this->len * 2);
	int x_pos = 0;
	
	char* D = new char[strlen(str) + 1]; // D - всё делимое (вся строка 8/10/16-base)
	memset(D, 0, strlen(str) + 1);
	char* result = new char[strlen(str) + 1]; // result - частное
	memset(result, 0, strlen(str) + 1);
	uint16 rest = 0; // rest - остаток
	// d - кусочек от делимого D, не должно превышать 0xFFFFFFFF (d_max)
	int d_size = (uint16) ceil((double)strlen("FFFFFFFF") * log10(16) / log10(base));
	char* d = new char[d_size + 1];
	char* d_max = new char[d_size + 1];
	sprintf(d_max, PRINTF_FORMAT, 0xFFFFFFFF);
	d_max[d_size] = '\0';
	
	//на первой итерации копируем str в делимое
	strcpy(D, str);
	
	while(true) {
		memset(result, 0, strlen(str) + 1); //очищаем частное
		rest = 0; //очищаем остаток
		
		for(int D_pos = 0, i = 0 ; D_pos < strlen(D) ; i++) {
			memset(d, 0, d_size + 1);
			if(rest != 0) //экономим 0 в начале d
				sprintf(d, PRINTF_FORMAT, rest); //в начало d записать остаток
			int d_len1 = strlen(d);
			int d_len2 = min(d_size - d_len1, (int)strlen(D) - D_pos);
			strncpy(d + d_len1, D + D_pos, d_len2); //потом записать след. порцию из D
			d[d_len1 + d_len2] = '\0';
			D_pos += d_len2;

			//если число в d превысит d_max, убрать последнюю цифру:
			if((d_len1 + d_len2) == d_size) {
				for(int j = 0 ; j < d_size ; j++) {
					if(d[j] > d_max[j]) { //d > d_max
						//убрать последнюю цифру:
						d_len2--;
						d[d_len1 + d_len2] = '\0';
						D_pos--;
						break;
					} else if(d[j] < d_max[j]) { //d < d_max
						break;
					}
				}
			}

			// преобразовать d в число
			uint32 d_int = strtoul(d, NULL, base);
			//записать частное дальше в result
			sprintf(result + strlen(result), PRINTF_FORMAT_2, (i == 0 ? 0 : d_len2), d_int / 0x10000);
			// остаток:
			rest = d_int % 0x10000;
		}

		this->x[x_pos++] = rest; //записать rest в this->x
		
		if(strtoul(result, NULL, base) == 0) //дальше не делится!
			break;
		
		//на след. итерации текущее частное превращается в новое делимое:
		memset(D, 0, strlen(str) + 1);
		strcpy(D, result);
	}

	this->no_zeros(); //сократим нули
}

/*
 * to string
 */
char* BigInt::toString(char** strToDelete/* = NULL*/, uint16 base/* = 0*/, bool with_prefix/* = true*/, bool with_sign/* = true*/, bool big/* = true*/) const
{
	if(base == 0)
		base = BigInt::baseOutput;
	
	assert(base > 0);
	
	int str_len = max(1, (int)ceil((double)this->len * 4 * log10(16) / log10(base)));
	int extra_len = (with_sign && this->neg && !this->is_zero() ? 1 : 0) + (with_prefix ? (base == 16 ? 2 : (base == 8 ? 1 : 0)) : 0);
	char* str = new char[str_len + extra_len + 1];
	memset(str, 0, str_len + extra_len + 1);
	
	BigInt result;
	uint16 rest;
	int i = 0, j = str_len + extra_len;
	do {
		result = BigInt::div((i == 0 ? *this : result), base, &rest);
		str[--j] = (rest < 10 ? ('0' + rest) : ((big ? 'A' : 'a') + rest - 10));
		i++;
	} while(!result.is_zero());
	
	//extra symbols
	if(with_prefix && base == 16) {
		str[--j] = 'x';
		str[--j] = '0';
	} else if(with_prefix && base == 8) {
		str[--j] = '0';
	}
	if(with_sign && this->neg && !this->is_zero()) {
		str[--j] = '-';
	}
	
	if(strToDelete != NULL)
		*strToDelete = str; //full string with leading zeros (zero-bytes, I mean), must be deleted
	
	return (str + j);
}

/*
 * operators
 */
BigInt& BigInt::operator=(const BigInt& b) {
	copy(b);
	return *this;
}

BigInt operator+(const BigInt& a, const BigInt& b) {
	return BigInt::add(a, b);
}
BigInt operator-(const BigInt& a, const BigInt& b) {
	return BigInt::sub(a, b);
}
BigInt operator*(const BigInt& a, const BigInt& b) {
	return BigInt::mul(a, b);
}
BigInt operator/(const BigInt& a, const BigInt& b) {
	return BigInt::div(a, b, NULL);
}
BigInt operator%(const BigInt& a, const BigInt& b) {
	return BigInt::mod(a, b);
}

bool operator==(const BigInt& a, const BigInt& b) {
	return (BigInt::cmp_sign(a, b) == 0);
}
bool operator!=(const BigInt& a, const BigInt& b) {
	return (BigInt::cmp_sign(a, b) != 0);
}
bool operator>(const BigInt& a, const BigInt& b) {
	return (BigInt::cmp_sign(a, b) > 0);
}
bool operator<(const BigInt& a, const BigInt& b) {
	return (BigInt::cmp_sign(a, b) < 0);
}
bool operator>=(const BigInt& a, const BigInt& b) {
	return (BigInt::cmp_sign(a, b) >= 0);
}
bool operator<=(const BigInt& a, const BigInt& b) {
	return (BigInt::cmp_sign(a, b) <= 0);
}

istream& operator>>(istream& _in, BigInt& a)
{
	char buf[512+1];
	memset(buf, 0, sizeof(buf) / sizeof(char));
	_in.getline(buf, sizeof(buf) / sizeof(char));
	a.fromString(buf);
	return _in;
}

ostream& operator<<(ostream& _out, const BigInt& a)
{
	char* strfull = NULL;
	char* str = a.toString(&strfull);
	_out << str;
	delete strfull;
	return _out;
}





//------------------------------------  MAIN: TESTS  ----------------------------------------


int main()
{
	BigInt a("6532764532547857900675090904854854850854673"), b("32362548628636543646545353");
	cout <<"a = " <<a <<endl;
	cout <<"b = " <<b <<endl;
	cout <<"add = " <<(a + b) <<endl;
	cout <<"sub = " <<(a - b) <<endl;
	cout <<"mul = " <<(a * b) <<endl;
	cout <<"div = " <<(a / b) <<endl;
	cout <<"mod = " <<(a % b) <<endl;
	return 0;
}


/*

*** TESTS RESULTS ***
a = 6532764532547857900675090904854854850854673
b = 32362548628636543646545353
add = 6532764532547857933037639533491398497400026
sub = 6532764532547857868312542276218311204309320
mul = 211416909864012129805401785310389961568330653581337773425035906484569
div = 201861868405729692
mod = 19688186967898047914133397


*** SAME TESTS ON PHP's BC MATH ***
<?php
	$a = "6532764532547857900675090904854854850854673";
	$b = "32362548628636543646545353";
	echo "a = " . $a . "<br/>";
	echo "b = " . $b . "<br/>";
	echo "add = " . bcadd($a, $b) . "<br/>";
	echo "sub = " . bcsub($a, $b) . "<br/>";
	echo "mul = " . bcmul($a, $b) . "<br/>";
	echo "div = " . bcdiv($a, $b) . "<br/>";
	echo "mod = " . bcmod($a, $b) . "<br/>";
?>

>>> OUTPUT:
a = 6532764532547857900675090904854854850854673
b = 32362548628636543646545353
add = 6532764532547857933037639533491398497400026
sub = 6532764532547857868312542276218311204309320
mul = 211416909864012129805401785310389961568330653581337773425035906484569
div = 201861868405729692
mod = 19688186967898047914133397
	
	
*/


