#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

double solve_multiply(char *expr, int len);
double solve_div(char *expr, int len);
double solve_minus(char *expr, int len);
double solve_plus(char *expr, int len);

char *find(char *str, char c, int len){
	int level = 0;

	for(int i = 0; i < len; i++, str++){
		if(*str == '(') level++;

		if(*str == ')') level--;

		if(*str == c && level == 0) return str;
	}

	return NULL;
}

void print(char *expr, int len){
	for (int i = 0; i < len; i++) {
		putchar(*expr++);
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

bool cmp(function *f, char *expr, int len){
	if(len < strlen(f->name) + 2){
		return false;
	}

	int i = 0;

	for(; i < strlen(f->name); i++){
		if(expr[i] != f->name[i]){
			return false;
		}
	}

	if(expr[len - 1] != ')'){
		return false;
	}

	return true;
}

function *tryToFind(char *expr, int len){
	for(int i = 0; i < sizeof(functions) / sizeof(function); i++){
		if(cmp(&functions[i], expr, len)){
			return &functions[i];
		}
	}

	return NULL;
}

double solve(char *expr, int len, double f(char *, int)){
	function *tmp = tryToFind(expr, len);
	if(tmp != NULL){
		for(int i = 0; i < len; i++){
			if(expr[i] == '('){
				expr += i;
				len -= i;
				break;
			}
		}

		return tmp->f(solve_plus(expr, len));
	}

	if(*expr != '(') return f(expr, len);
    if(*(expr + len - 1) != ')') return f(expr, len);

     return solve_plus(expr + 1, len - 2);

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
double n(char *expr, int len){
	double tmp = 0;
	bool beforePoint = true;
	double division = 0.1;
	for(int i = 0; i < len; i++){
		if(*expr == '.'){
			if(!beforePoint) error("Too much decimal points used!");
			beforePoint = false;
			continue;
		}

		if(!isdigit(*expr)){
			error("Converted number is not a digit!");
		}

		if(beforePoint) tmp = tmp * 10 + *expr - '0';
		else {
			tmp += (*expr - '0') * division;
			division *= 0.1;
		}
		expr++;
	}
	return tmp;
}

double solve_multiply(char *expr, int len){
	char *ptr = find(expr, '*', len);
	if(ptr == NULL){
		return solve(expr, len, n);
	}

	return solve(expr, ptr - expr, solve_multiply) * solve(ptr + 1, len - (ptr - expr) - 1, solve_multiply);
}

double solve_div(char *expr, int len){
	char *ptr = find(expr, '/', len);
	if(ptr == NULL){
		return solve(expr, len, solve_multiply);
	}

	return solve(expr, ptr - expr, solve_div) / solve(ptr + 1, len - (ptr - expr) - 1, solve_div);
}


double solve_minus(char *expr, int len){
	char *ptr = find(expr, '-', len);
	if(ptr == NULL){
		return solve(expr, len, solve_div);
	}

	return solve(expr, ptr - expr, solve_minus) - solve(ptr + 1, len - (ptr - expr) - 1, solve_minus);
}

double solve_plus(char *expr, int len){
	char *ptr = find(expr, '+', len);
	if(ptr == NULL){
		return solve(expr, len, solve_minus);
	}

	return solve(expr, ptr - expr, solve_plus) + solve(ptr + 1, len - (ptr - expr) - 1, solve_plus);
}

double pieSolver(char *expr){
	return solve_plus(expr, strlen(expr));
}
