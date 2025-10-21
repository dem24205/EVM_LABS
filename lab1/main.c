#include "math.h"
#include <stdio.h>

#define ARG M_PI/6.0

double calc_exp(double arg, int accur) {
    double result = 1.0;
    double term = 1.0;
    for (int n = 0; n < accur-1; ++n) {
        term *= arg/(n+1);
        result += term;
    }
    return result;
}

int main(void)
{
    int N = 2000000000;
    double result = calc_exp(ARG, N);
    printf("%.15f\n", result);
    return 0;
}
