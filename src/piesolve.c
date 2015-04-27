/**
 * @file piesolve.c
 *
 * @brief Basic parser for arithmetic expressions - uses piemath as backend math library
 * @author Ondřej Budai <ondrej@budai.cz>
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "piemath.h"

/// Internal buffer for storing error messages
char error[1000] = "";

/// Internal buffer for mathematic expressions (used when piesolve has to fill in ending brackets)
char buffer[1000] = "";

/**
 * Structure to hold expressions
 * It's main advantage is that I can split expression without copying it somewhere else.
 * The split operation is very simple - you can just move the pointer or modify the length (or both)
 */
typedef struct {
	/// Pointer to string start
	char *p;
	/// Length of string
	size_t len;
} string;

double solve_plus(string expr);

/**
 * Finds c in string expr
 *
 * @param expr String to search in
 * @param c Searched character
 * @param fromEnd Search direction - when true the search is performed from the end of expr, from the start otherwise
 * @return Pointer to found character, if search was unsuccessful returns NULL
 */
char *find(string expr, char c, bool fromEnd){
	int level = 0;

	if(fromEnd){
		expr.p += expr.len - 1;
	}

	for(size_t i = 0; i < expr.len; i++){
		if(*(expr.p) == '(') level++;
		else if(*(expr.p) == ')') level--;
		if(*(expr.p) == c && level == 0) return expr.p;

		expr.p += fromEnd ? -1 : 1;
	}

	return NULL;
}

/**
 * Structure used to translate name in string in actual function
 */
typedef struct {
	/// The name of function
	char *name;
	/// Pointer to the function
	double (*f)(double);
} function;

/// Translation table from string names of function to pointers to implemented functions
function functions[] = {
	{"sqrt", piemathSqrt},
	{"log",  piemathLog},
	{"ln",   piemathLn},
	{"cbrt", piemathCbrt},
	{"exp",  piemathExp},
	{"rand", piemathRand}
};

/**
 * Searches whether function f is used in string expr
 * 
 * @param f Searched function
 * @param expr The string to search in
 * @return True is function was found, false otherwise
 */
bool searchForFunction(function *f, string expr){
	if(expr.len < strlen(f->name) + 2){
		return false;
	}

	size_t i = 0;

	for(; i < strlen(f->name); i++){
		if(expr.p[i] != f->name[i]){
			return false;
		}
	}

	if(expr.p[expr.len - 1] != ')'){
		return false;
	}

	return true;
}

/**
 * Tries to find any functions in expr
 * 
 * @param expr The string to search in
 * @return Pointer to definition of found function
 */
function *tryToFindFunction(string expr){
	for(size_t i = 0; i < sizeof(functions) / sizeof(function); i++){
		if(searchForFunction(&functions[i], expr)){
			return &functions[i];
		}
	}

	return NULL;
}

/**
 * Function which converts internal string to double, only positive number are supported (minus is handled separately)
 *
 * @param expr String to convert
 * @return Returns expr converted to double
 */
double n(string expr){
	double tmp = 0;
	bool beforePoint = true;
	double division = 0.1;

	if(expr.len == 1 && *(expr.p) == 'e'){
		return piemathE();
	}

	for(size_t i = 0; i < expr.len; i++){
		if(*(expr.p) == '.'){
			if(!beforePoint){
				strcpy(error, "Too much decimal points used!");
				return NAN;
			}
			beforePoint = false;
			expr.p++;
			continue;
		}

		if(!isdigit(*(expr.p))){
			strcpy(error, "Converted number is not a digit!");
			return NAN;
		}

		if(beforePoint) tmp = tmp * 10 + *(expr.p) - '0';
		else {
			tmp += (*(expr.p) - '0') * division;
			division *= 0.1;
		}
		expr.p++;
	}
	return tmp;
}

/**
 * Splits string into two strings - the delimiter is determined by pointer to it
 *
 * @param what String to split
 * @param delimiter Pointer to delimiter - the delimiter is discarded and not use in left part nor right part
 * @param l Left part of split string
 * @param r Right part of split string
 */
void split(string what, char *delimiter, string *l, string *r){
	l->p = what.p;
	l->len = delimiter - what.p;

	r->p = delimiter + 1;
	r->len = what.len - (delimiter - what.p) - 1;
}

/**
 * Final level of solve - checks for brackets and function
 * If brackets or function was found use 1st level solve for inside and return the value modified by specified function
 * Otherwise try to convert the expression to number and return it
 *
 * @param expr Expression to solve
 * @return Computed value
 */
double solve(string expr){
	function *tmp = tryToFindFunction(expr);
	if(tmp != NULL){
		for(size_t i = 0; i < expr.len; i++){
			if(expr.p[i] == '('){
				expr.p += i;
				expr.len -= i;
				break;
			}
		}

		return tmp->f(solve_plus(expr));
	}

	if(*(expr.p) != '(' || *(expr.p + expr.len - 1) != ')') return n(expr);

    return solve_plus((string){expr.p + 1, expr.len - 2});

}

/**
 * 6th level of solve - tries to find factorial sign, if found, split expr by it and try again for both, else fallthrough to 7th level
 * Also checks if the number to calculate factorial is a positive integer - otherwise throw an error
 *
 * @param expr Expression to solve
 * @return Computed value
 */
double solve_fact(string expr){
	char *ptr = find(expr, '!', true);
	if(ptr == NULL){
		return solve(expr);
	}

	string l, r;
	split(expr, ptr, &l, &r);

	if(r.len != 0){
		return solve(expr);
	}

	if(l.len == 0){
		strcpy(error, "Factorial with no argument!");
		return NAN;
	}

	double number = solve_fact(l);

	if(fabs(round(number) - number) > 0.00000001){
		strcpy(error, "Factorial requires integer!");
		return NAN;
	}

	double rNumber = round(number);

	if(rNumber < 0){
		strcpy(error, "Factorial requite positive integer!");
		return NAN;
	}

	if(rNumber > 65){
		strcpy(error, "Number to calculate factorial is too big!");
		return NAN;
	}

	return piemathFact((unsigned int)rNumber);
}

/**
 * 5th level of solve - tries to find multiplication sign, if found, split expr by it and try again for both, else fallthrough to 6th level
 *
 * @param expr Expression to solve
 * @return Computed value
 */
double solve_pow(string expr){
	char *ptr = find(expr, '^', true);
	if(ptr == NULL){
		return solve_fact(expr);
	}

	string l, r;
	split(expr, ptr, &l, &r);

	if(l.len == 0 || r.len == 0){
		strcpy(error, "Operand error!");
		return NAN;
	}

	return piemathPower(solve_pow(l), solve_pow(r));
}

/**
 * 4th level of solve - tries to find multiplication sign, if found, split expr by it and try again for both, else fallthrough to 5th level
 *
 * @param expr Expression to solve
 * @return Computed value
 */
double solve_multiply(string expr){
	char *ptr = find(expr, '*', false);
	if(ptr == NULL){
		return solve_pow(expr);
	}

	string l, r;
	split(expr, ptr, &l, &r);

	if(l.len == 0 || r.len == 0){
		strcpy(error, "Operand error!");
		return NAN;
	}

	return piemathMultiply(solve_multiply(l), solve_multiply(r));
}

/**
 * 3rd level of solve - tries to find division sign, if found, split expr by it and try again for both, else fallthrough to 4th level
 *
 * @param expr Expression to solve
 * @return Computed value
 */
double solve_div(string expr){
	char *ptr = find(expr, '/', false);
	if(ptr == NULL){
		return solve_multiply(expr);
	}

	string l, r;
	split(expr, ptr, &l, &r);

	if(l.len == 0 || r.len == 0){
		strcpy(error, "Operand error!");
		return NAN;
	}

	return piemathDivide(solve_div(l), solve_div(r));
}

/**
 * 2nd level of solve - tries to find subtraction sign, if found, split expr by it and try again for both, else fallthrough to 3rd level
 *
 * @param expr Expression to solve
 * @return Computed value
 */
double solve_minus(string expr){
	char *ptr = find(expr, '-', false);
	if(ptr == NULL){
		return solve_div(expr);
	}

	string l, r;
	split(expr, ptr, &l, &r);

	if(r.len == 0){
		strcpy(error, "Operand error!");
		return NAN;
	}

	return piemathSubtract(solve_minus(l), solve_minus(r));
}

/**
 * 1st level of solve - tries to find addition sign, if found, split expr by it and try again for both, else fallthrough to 2nd level
 *
 * @param expr Expression to solve
 * @return Computed value
 */
double solve_plus(string expr){
	char *ptr = find(expr, '+', false);
	if(ptr == NULL){
		return solve_minus(expr);
	}

	string l, r;
	split(expr, ptr, &l, &r);

	if(r.len == 0){
		strcpy(error, "Operand error!");
		return NAN;
	}

	return piemathAdd(solve_plus(l), solve_plus(r));
}

/**
 * Fills in missing ending brackets
 *
 * @param expr Expression to fill in ending brackets
 * @return Expression with filled ending brackets
 */
char *checkBrackets(char *expr){
	int brackets = 0;

	char *p = expr;

	while(*p){
		if(*p == '(') brackets++;
		else if(*p == ')'){
			brackets--;
			if(brackets < 0) return NULL;
		}
		p++;
	}

	if(brackets == 0){
		return expr;
	}

	strcpy(buffer, expr);
	for(int i = 0; i < brackets; i++){
		strcat(buffer, ")");
	}

	return buffer;
}

/**
 * Tries to solve mathematically given expression
 * If solve failes, returns NAN and the error message can be get with getError function
 *
 * @param expr String to expression
 * @return Computed value, NAN in case of failure
 */
double pieSolver(char *expr){
	char *edited = checkBrackets(expr);

	if(edited == NULL){
		strcpy(error, "Bracket error!");
		return NAN;
	}

	return solve_plus((string){edited, strlen(edited)});
}

/**
 * Returns error message generated by piesolve library
 *
 * @return PieSolve error message
 */
char *getError(){
	return error;
}
