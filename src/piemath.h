/**
 * @file piemath.h
 *
 * @brief Library with basic arithmetic operations
 * @author Ondřej Budai <ondrej@budai.cz>
 */

#ifndef PIEMATH_H
#define PIEMATH_H

double piemathAdd(double a, double b);
double piemathSubtract(double a, double b);
double piemathMultiply(double a, double b);
double piemathDivide(double a, double b);
double piemathLn(double a);
double piemathLog(double a);
double piemathSqrt(double a);
double piemathCbrt(double a);
double piemathSquare(double a);
double piemathPower(double a, double b);
double piemathExp(double a);
unsigned long long piemathFact(unsigned int a);
double piemathE();
double piemathRand();

#endif
