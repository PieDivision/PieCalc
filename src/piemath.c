#include <math.h>

#include "piemath.h"

double piemathAdd(double a, double b){
	return a + b;
}
double piemathSubstract(double a, double b){
	return a - b;
}
double piemathMultiply(double a, double b){
	return a * b;
}
double piemathDivide(double a, double b){
	if(b == 0){
		return NAN;
	}

	return a / b;
}
double piemathLn(double a){
	if(a <= 0){
		return NAN;
	}
	return log(a);
}
double pieMathLog(double a){
	if(a <= 0){
		return NAN;
	}
	return log10(a);
}
double piemathSqrt(double a){
	if(a < 0){
		return NAN;
	}
	return sqrt(a);
}
double piemathCbrt(double a){
	return cbrt(a);
}
double piemathSquare(double a){
	return pow(a, 2);
}
double piemathPower(double a, int power){
	return pow(a, power);
}
double piemathExp(double a){
	return exp(a);
}

unsigned long long piemathFact(unsigned int a){
	unsigned long long fact = 1;
	for(; a>0; a--){
		fact *= a;
	}

	return fact;
}
