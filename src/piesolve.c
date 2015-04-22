#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "piemath.h"

char error[1000] = "";
char buffer[1000] = "";

typedef struct {
	char *p;
	size_t len;
} string;

double solve_multiply(string expr);
double solve_div(string expr);
double solve_minus(string expr);
double solve_plus(string expr);

char *find(string expr, char c, bool right){
	int level = 0;

	if(right){
		expr.p += expr.len - 1;
	}

	for(size_t i = 0; i < expr.len; i++){
		if(*(expr.p) == '(') level++;
		else if(*(expr.p) == ')') level--;
		if(*(expr.p) == c && level == 0) return expr.p;

		expr.p += right ? -1 : 1;
	}

	return NULL;
}

void print(string expr){
	for (size_t i = 0; i < expr.len; i++) {
		putchar(*(expr.p)++);
	}
	putchar('\n');
}

typedef struct {
	char *name;
	double (*f)(double);
} function;

function functions[] = {
	{"sqrt", sqrt},
	{"log", log10},
	{"ln", log},
	{"cbrt", cbrt},
	{"exp", exp},
	{"rand", piemathRand}
};

bool cmp(function *f, string expr){
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

function *tryToFind(string expr){
	for(size_t i = 0; i < sizeof(functions) / sizeof(function); i++){
		if(cmp(&functions[i], expr)){
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

void split(string what, char *delimiter, string *l, string *r){
	l->p = what.p;
	l->len = delimiter - what.p;

	r->p = delimiter + 1;
	r->len = what.len - (delimiter - what.p) - 1;
}

double solve(string expr){
	function *tmp = tryToFind(expr);
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

	return piemathFact((unsigned int)round(number));
}

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

	return pow(solve_pow(l), solve_pow(r));
}

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

	return solve_multiply(l) * solve_multiply(r);
}

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

	return solve_div(l) / solve_div(r);
}


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

	return solve_minus(l) - solve_minus(r);
}

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

	return solve_plus(l) + solve_plus(r);
}

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

double pieSolver(char *expr){
	char *edited = checkBrackets(expr);

	if(edited == NULL){
		strcpy(error, "Bracket error!");
		return NAN;
	}

	return solve_plus((string){edited, strlen(edited)});
}

char *getError(){
	return error;
}
