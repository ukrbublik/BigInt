#include "bigint.h"

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
