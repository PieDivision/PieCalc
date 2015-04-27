/**
 * @file piemath.c
 *
 * @brief Library with basic arithmetic operations
 * @author Ondřej Budai <ondrej@budai.cz>
 */

#include <math.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>

#include "piemath.h"

/// Used for tracking whether srand() was called or not
bool randInitialized = false;

#ifndef M_E
/// Euler's constant
#define M_E 2.7182818284590452354
#endif


/**
 * Adds two number: a+b
 * @param a Number a
 * @param b Number b
 * @return a+b
 */
double piemathAdd(double a, double b){
	return a + b;
}

/**
 * Subtracts two number: a-b
 * @param a Number a
 * @param b Number b
 * @return a-b
 */
double piemathSubtract(double a, double b){
	return a - b;
}

/**
 * Multiplies two number: a*b
 * @param a Number a
 * @param b Number b
 * @return a*b
 */
double piemathMultiply(double a, double b){
	return a * b;
}

/**
 * Divides two number: a/b
 * @param a Number a
 * @param b Number b
 * @return a/b, NAN in case when b = 0
 */
double piemathDivide(double a, double b){
	if(b == 0){
		return NAN;
	}

	return a / b;
}

/**
 * Calculates natural logarithm of number a
 * @param a Number a
 * @return Natural logithm of number a, NAN in case when a<=0
 */
double piemathLn(double a){
	if(a <= 0){
		return NAN;
	}
	return log(a);
}

/**
 * Calculates decadic logarithm of number a
 * @param a Number a
 * @return Decadic logithm of number a, NAN in case when a<=0
 */
double piemathLog(double a){
	if(a <= 0){
		return NAN;
	}
	return log10(a);
}

/**
 * Calculates square root of number a
 * @param a Number a
 * @return Square root of number a, NAN in case when a<0
 */
double piemathSqrt(double a){
	if(a < 0){
		return NAN;
	}
	return sqrt(a);
}

/**
 * Calculates cubic root of number a
 * @param a Number a
 * @return Cubic root of number a
 */
double piemathCbrt(double a){
	return cbrt(a);
}

/**
 * Calculates number a squared
 * @param a Number a
 * @return Number a squred
 */
double piemathSquare(double a){
	return pow(a, 2);
}

/**
 * Calculates a to the power of b
 * @param a Number a
 * @param a Number b
 * @return Number a to the power of b
 */
double piemathPower(double a, double b){
	return pow(a, b);
}

/**
 * Calculates e (uler's constant) to the power of a
 * @param a Number a
 * @return Number e to the power of a
 */
double piemathExp(double a){
	return exp(a);
}

/**
 * Calculates a factorial
 * @param a Number a
 * @return Factorial of number a
 */
unsigned long long piemathFact(unsigned int a){
	unsigned long long fact = 1;
	for(; a>0; a--){
		fact *= a;
	}

	return fact;
}

/**
 * Returns Euler's constant
 * @return Euler's constant
 */
double piemathE(){
	return M_E;
}

/**
 * Returns pseudorandom number from 0.0 to 1.0, the generator is automatically initalized with current time
 * @return Pseudorandom number in range from 0.0 to 1.0
 */
double piemathRand(){
	if(!randInitialized){
		srand(time(0));
		randInitialized = true;
	}

	return ((double)rand()) / RAND_MAX;
}
