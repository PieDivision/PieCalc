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

	// If we're searching from the end, invert the pointer
	if(fromEnd) expr.p += expr.len - 1;

	for(size_t i = 0; i < expr.len; i++){
		// Handle bracket levels
		if(*(expr.p) == '(') level++;
		else if(*(expr.p) == ')') level--;

		// If we have found the correct char and we are not in any brackets return current pointer
		if(*(expr.p) == c && level == 0) return expr.p;

		// Move the pointer according to correct direction
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
};

/**
 * Searches whether function f is used in string expr
 * 
 * @param f Searched function
 * @param expr The string to search in
 * @return True is function was found, false otherwise
 */
bool searchForFunction(function *f, string expr){
	// If the expression is longer than function name plus brackets, it's certainly not there
	if(expr.len < strlen(f->name) + 2) return false;

	// Search if expression starts with function name
	size_t i = 0;
	for(; i < strlen(f->name); i++){
		if(expr.p[i] != f->name[i]){
			return false;
		}
	}

	// Check for starting and ending bracket
	if(expr.p[i] != '(' || expr.p[expr.len - 1] != ')'){
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
	// Iterate over all functions, if a function is found return its pointer
	for(size_t i = 0; i < sizeof(functions) / sizeof(function); i++){
		if(searchForFunction(&functions[i], expr)){
			return &functions[i];
		}
	}

	return NULL;
}

/**
 * Compares string with c-string
 *
 * @param expr String A
 * @param str String B
 * @return True if both strings are equal, false otherwise
 */
bool stringCompare(string expr, char *str){
	// If the lengths are not the same, they cannot be equal
	if(expr.len != strlen(str)) return false;

	// Iterate over all characters - if one of them are not equal, return false
	for(size_t i = 0; i < strlen(str); i++){
		if(expr.p[i] != str[i]) return false;
	}

	return true;
}

/**
 * Function which converts internal string to double, only positive number are supported (minus is handled separately)
 *
 * @param expr String to convert
 * @return Returns expr converted to double
 */
double n(string expr){
	// Handle Euler's number
	if(expr.len == 1 && *(expr.p) == 'e'){
		return piemathE();
	}

	// Handle random
	if(stringCompare(expr, "rand")){
		return piemathRand();
	}


	double tmp = 0;
	bool beforePoint = true;
	double division = 0.1;

	for(size_t i = 0; i < expr.len; i++){
		// If we are currently crossing decimal point
		if(*(expr.p) == '.'){
			// Throw an error if there are more decimal points
			if(!beforePoint){
				strcpy(error, "Too much decimal points used!");
				return NAN;
			}
			// Set the decimal part marker
			beforePoint = false;
			expr.p++;
			continue;
		}

		// If we find non-digit characer, throw an error
		if(!isdigit(*(expr.p))){
			strcpy(error, "Converted number is not a digit!");
			return NAN;
		}

		// Add converted character
		if(beforePoint){
			tmp = tmp * 10 + *(expr.p) - '0';
		} else {
			tmp += (*(expr.p) - '0') * division;
			division *= 0.1;
		}
		// Move to the next character
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
	// The left side has the same pointer
	l->p = what.p;
	l->len = delimiter - what.p;

	// The right side has the delimiter pointer incremented by one
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
	// Try to find function in expression
	function *tmp = tryToFindFunction(expr);

	// If function was found
	if(tmp != NULL){
		// Strip the function name and brackets - we don't need them anymore
		for(size_t i = 0; i < expr.len; i++){
			if(expr.p[i] == '('){
				expr.p += i + 1;
				expr.len -= i + 2;
				break;
			}
		}
		// Calculate the inner part and return the value modified by appropriate function
		return tmp->f(solve_plus(expr));
	}

	// If there are no brackets around the expression, return the converted number
	if(*(expr.p) != '(' || *(expr.p + expr.len - 1) != ')') return n(expr);

	// Otherwise strip the brackets and start again
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
	// Try to find !, if nout found, fallthrough
	char *ptr = find(expr, '!', true);
	if(ptr == NULL){
		return solve(expr);
	}

	string l, r;
	split(expr, ptr, &l, &r);

	// If there's something on the right side, we cannot solve it just yet -> fallthrough
	if(r.len != 0){
		return solve(expr);
	}

	// We must have argument to factorial
	if(l.len == 0){
		strcpy(error, "Factorial with no argument!");
		return NAN;
	}

	// Process left side
	double number = solve_fact(l);


	// If it's not integer, we cannot factorial it, in that case throw an error
	if(fabs(round(number) - number) > 0.00000001){
		strcpy(error, "Factorial requires integer!");
		return NAN;
	}

	// Round the number precisely
	double rNumber = round(number);

	// Do not factorial negative numbers
	if(rNumber < 0){
		strcpy(error, "Factorial requite positive integer!");
		return NAN;
	}

	// Upper limit - bigger numbers are just too huge
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
	// If we cannot find appropriate sign, fallthrough
	if(ptr == NULL){
		return solve_fact(expr);
	}

	// Split the expression
	string l, r;
	split(expr, ptr, &l, &r);

	// Left and right side cannot be zero-length
	if(l.len == 0 || r.len == 0){
		strcpy(error, "Operand error!");
		return NAN;
	}

	// Try to solve left and side at the same level and return the value modified by currently processed function
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
	// If we cannot find appropriate sign, fallthrough
	if(ptr == NULL){
		return solve_pow(expr);
	}

	// Split the expression
	string l, r;
	split(expr, ptr, &l, &r);

	// Left and right side cannot be zero-length
	if(l.len == 0 || r.len == 0){
		strcpy(error, "Operand error!");
		return NAN;
	}

	// Try to solve left and side at the same level and return the value modified by currently processed function
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
	// If we cannot find appropriate sign, fallthrough
	if(ptr == NULL){
		return solve_multiply(expr);
	}

	// Split the expression
	string l, r;
	split(expr, ptr, &l, &r);

	// Left and right side cannot be zero-length
	if(l.len == 0 || r.len == 0){
		strcpy(error, "Operand error!");
		return NAN;
	}

	// Try to solve left and side at the same level and return the value modified by currently processed function
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
	// If we cannot find appropriate sign, fallthrough
	if(ptr == NULL){
		return solve_div(expr);
	}

	// Split the expression
	string l, r;
	split(expr, ptr, &l, &r);

	// Right side cannot be zero-lengths
	if(r.len == 0){
		strcpy(error, "Operand error!");
		return NAN;
	}

	// Try to solve left and side at the same level and return the value modified by currently processed function
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
	// If we cannot find appropriate sign, fallthrough
	if(ptr == NULL){
		return solve_minus(expr);
	}

	// Split the expression
	string l, r;
	split(expr, ptr, &l, &r);

	// Right side cannot be zero-lengthss
	if(r.len == 0){
		strcpy(error, "Operand error!");
		return NAN;
	}

	// Try to solve left and side at the same level and return the value modified by currently processed function
	return piemathAdd(solve_plus(l), solve_plus(r));
}

/**
 * Fills in missing ending brackets
 *
 * @param expr Expression to fill in ending brackets
 * @return Expression with filled ending brackets
 */
char *fillBrackets(char *expr){
	int brackets = 0;

	char *p = expr;

	// Iterate over expression
	while(*p){
		// Handle starting and ending brackets
		if(*p == '(') brackets++;
		else if(*p == ')'){
			brackets--;
			// It the current count of brackets is negative, something - expression is invalid, return NULL
			if(brackets < 0) return NULL;
		}
		p++;
	}

	// If total bracker count is zero, return the original expression
	if(brackets == 0) return expr;

	// Otherwise copy the expression to buffer and fill in missing ending brackets
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
	// Check brackets and try to fill in ending brackets if needed
	char *edited = fillBrackets(expr);

	// If the brackets are wrong, set the error message and return NAN
	if(edited == NULL){
		strcpy(error, "Bracket error!");
		return NAN;
	}

	// Convert the expression to internal string format and begin its processing on the first level
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
