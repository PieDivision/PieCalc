#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

double solve_multiply(char *expr, int len);
double solve_div(char *expr, int len);
double solve_minus(char *expr, int len);
double solve_plus(char *expr, int len);

char *find(char *str, char c, int len, bool right){
	int level = 0;

	if(right){
		str += len - 1;
	}

	for(int i = 0; i < len; i++){
		if(*str == '(') level++;
		else if(*str == ')') level--;
		if(*str == c && level == 0) return str;

		str += right ? -1 : 1;
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

double solve(char *expr, int len){
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

	if(*expr != '(' || *(expr + len - 1) != ')') return n(expr, len);

    return solve_plus(expr + 1, len - 2);

}

double solve_pow(char *expr, int len){
	char *ptr = find(expr, '*', len, true);
	if(ptr == NULL){
		return solve(expr, len);
	}

	return pow(solve_pow(expr, ptr - expr), solve_pow(ptr + 1, len - (ptr - expr) - 1));
}

double solve_multiply(char *expr, int len){
	char *ptr = find(expr, '*', len, false);
	if(ptr == NULL){
		return solve_pow(expr, len);
	}

	return solve_multiply(expr, ptr - expr) * solve_multiply(ptr + 1, len - (ptr - expr) - 1);
}

double solve_div(char *expr, int len){
	char *ptr = find(expr, '/', len, false);
	if(ptr == NULL){
		return solve_multiply(expr, len);
	}

	return solve_div(expr, ptr - expr) / solve_div(ptr + 1, len - (ptr - expr) - 1);
}


double solve_minus(char *expr, int len){
	char *ptr = find(expr, '-', len, false);
	if(ptr == NULL){
		return solve_div(expr, len);
	}

	return solve_minus(expr, ptr - expr) - solve_minus(ptr + 1, len - (ptr - expr) - 1);
}

double solve_plus(char *expr, int len){
	char *ptr = find(expr, '+', len, false);
	if(ptr == NULL){
		return solve_minus(expr, len);
	}

	return solve_plus(expr, ptr - expr) + solve_plus(ptr + 1, len - (ptr - expr) - 1);
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

	return solve_plus(expr, strlen(expr));
}
