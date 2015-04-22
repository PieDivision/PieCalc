/**
 * Tests for the library piemath.h
 * TODO: otestovat hranice, nuly, NAN, INF
 */


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include "../src/piemath.h"
#include "../src/piesolve.h"

bool success = true;
int ok = 0;
int failed = 0;


void print_success(char *text)
{
	printf("Test OK: %s\n", text);
}

void print_failure(char *text)
{
	printf("Test FAILED: %s\n", text);
}

void assert_equal(char *text, double pievalue, double correctvalue)
{
	if(pievalue == correctvalue)
	{
		print_success(text);
		ok++;
	}
	else
	{
		print_failure(text);
		success = false;
		failed++;
	}
}

int main()
{
	printf("*****************************************\n");
	printf("Testing piemath.h...\n");
	printf("*****************************************\n");

	assert_equal("Addition", piemathAdd(4, 2), 6);

	assert_equal("Subtraction", piemathSubtract(4, 2), 2);

	assert_equal("Integer multiplication", piemathMultiply(4, 2), 8);
	assert_equal("Double multiplication", piemathMultiply(4.5, 2.2), (double)9.9);
	assert_equal("Multiplication by zero", piemathMultiply(4, 0), 0);

	assert_equal("Integer division", piemathDivide(4, 2), 2);
	assert_equal("Double division", piemathDivide(4.5, 2.2), (double)(4.5/2.2));
	assert_equal("Division by zero", piemathDivide(4, 0), NAN);

	assert_equal("Natural logarithm", piemathLn(3), log(3));
	assert_equal("Natural logarithm - NAN test", piemathLn(0), NAN);

	assert_equal("Logarithm 10", pieMathLog(1000), log10(1000));
	assert_equal("Logarithm 10 - NAN test", pieMathLog(-1), NAN);

	assert_equal("Integer square root", piemathSqrt(25), 5);
	assert_equal("Double square root", piemathSqrt(213), sqrt(213));
	assert_equal("Square root - NAN test", piemathSqrt(-5), NAN);

	assert_equal("Integer cubic root", piemathCbrt(27), 3);
	assert_equal("Double cubic root", piemathCbrt(45.5), cbrt(45.5));

	assert_equal("Square", piemathSquare(4), 16);
	assert_equal("Power", piemathPower(2, 5), 32);
	assert_equal("Exponential", piemathExp(4), exp(4));

	assert_equal("Factorial", piemathFact(5), 120);

	printf("*****************************************\n");
	printf("Testing piesolve.h...\n");
	printf("*****************************************\n");

	printf("*****************************************\n");
	printf("%d tests, OK %d, failed %d\n", (ok+failed), ok, failed);
	printf("*****************************************\n");


	return success ? EXIT_SUCCESS : EXIT_FAILURE;
}