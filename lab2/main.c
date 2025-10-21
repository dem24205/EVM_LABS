#include <math.h>
#include <stdio.h>
#include <time.h>

#define TEST_CASES 3

double calc_exp(double x, long int accur) {
    double result = 1.0;
    double term = 1.0;
    for (long int n = 1; n < accur; ++n) {
        term *= x / n;
        result += term;
    }
    return result;
}

void print_absolute_error(double arg) {
    int accuracy[] = {10, 100, 1000};
    for (int i = 0; i < TEST_CASES; ++i) {
        double my_exp = calc_exp(arg, accuracy[i]);
        double lib_exp = exp(arg);
        printf("N=%d: absolute error = %.2e\n",
               accuracy[i], fabs(my_exp - lib_exp));
    }
    printf("\n");
}

void print_result(long int n, double result, double time_taken) {
    printf("N=%ld: result=%.15f, time=%.6f seconds\n",
           n, result, time_taken);
}

void measure_performance(double arg, long int* accuracies) {
    struct timespec start, end;
    double time_taken, result;
    for (int i = 0; i < TEST_CASES; ++i) {
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        result = calc_exp(arg, accuracies[i]);
        clock_gettime(CLOCK_MONOTONIC_RAW, &end);

        time_taken = (end.tv_sec - start.tv_sec) +
                     (end.tv_nsec - start.tv_nsec) * 1e-9;

        print_result(accuracies[i], result, time_taken);
    }
}

int main(void) {
    double arg = M_PI / 6.0;
    print_absolute_error(arg);
    long int accuracies[] = {4000000000, 6000000000, 8000000000};
    measure_performance(arg, accuracies);
    return 0;
}
