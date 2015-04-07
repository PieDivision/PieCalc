#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

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
	{"sqrt", sqrt}
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



void error(char *msg){
	fprintf(stderr, "%s\n", msg);
	exit(EXIT_FAILURE);
}

/**
 * Function which converts internal string to double, only positive number are supported (minus is handled separately)
 *
 * @param expr Pointer to string
 * @param len Length of string
 * @return Returns expr converted to double
 */
double n(string expr){
	double tmp = 0;
	bool beforePoint = true;
	double division = 0.1;

	for(size_t i = 0; i < expr.len; i++){
		if(*(expr.p) == '.'){
			if(!beforePoint) error("Too much decimal points used!");
			beforePoint = false;
			continue;
		}

		if(!isdigit(*(expr.p))){
			error("Converted number is not a digit!");
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

double solve_pow(string expr){
	char *ptr = find(expr, '^', true);
	if(ptr == NULL){
		return solve(expr);
	}

	string l, r;
	split(expr, ptr, &l, &r);

	if(l.len == 0 || r.len == 0){
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
		return NAN;
	}

	return solve_plus(l) + solve_plus(r);
}

bool checkBrackets(char *expr){
	int brackets = 0;
	while(*expr){
		if(*expr == '(') brackets++;
		else if(*expr == ')'){
			brackets--;
			if(brackets < 0) return false;
		}
		expr++;
	}

	return true;
}

double pieSolver(char *expr){
	if(!checkBrackets(expr)){
		return NAN;
	}

	return solve_plus((string){expr, strlen(expr)});
}
