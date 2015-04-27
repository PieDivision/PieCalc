/**
 * @file piesolve.h
 *
 * @brief Basic parser for arithmetic expressions - uses piemath as backend math library
 * @author Ondřej Budai <ondrej@budai.cz>
 */

#ifndef PIE_SOLVE_H
#define PIE_SOLVE_H

double pieSolver(char *expr);
char *getError();

#endif
