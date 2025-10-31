#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef double (*funker)(double, double);

typedef struct {
    char chara;
    funker func;
} Operation;

double i_add(double arg1, double arg2) { return arg1 + arg2; }
double i_sub(double arg1, double arg2) { return arg1 - arg2; }
double i_mul(double arg1, double arg2) { return arg1 * arg2; }
double i_div(double arg1, double arg2) { return arg1 / arg2; }

Operation ops[] = {
    {'+', i_add},
    {'-', i_sub},
    {'*', i_mul},
    {'/', i_div}
};