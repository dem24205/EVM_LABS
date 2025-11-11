#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>
#include <time.h>

#define MIN_ARRAY_SIZE 128
#define MAX_ARRAY_SIZE (32 * 256 * 1024)
#define CIRCLE_IT 7  // Нечётное количество для медианы

uint64_t read_tsc() {
    uint32_t lo, hi;
    asm volatile("rdtsc" : "=a"(lo), "=d"(hi));
    return ((uint64_t)hi << 32) | lo;
}

void fill_forward(int *array, int N) {
    for (int i = 0; i < N; ++i)
        array[i] = (i + 1) % N;
}

void fill_backward(int *array, int N) {
    for (int i = 0; i < N; ++i)
        array[i] = (i == 0) ? N - 1 : i - 1;
}

void fill_random(int *array, int N) {
    for (int i = 0; i < N; ++i) array[i] = i;
    for (int i = N - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        int tmp = array[i];
        array[i] = array[j];
        array[j] = tmp;
    }
}

void warm_up(int *array, int N) {
    volatile int k = 0;
    for (int i = 0; i < N; ++i)
        k = array[k];
}

double full_pass(int *array, int N, int K) {
    volatile int k = 0;
    uint64_t start = read_tsc();
    for (int i = 0; i < N * K; ++i)
        k = array[k];
    uint64_t end = read_tsc();
    return (double)(end - start) / (N * K);
}

int get_optimal_K(int N) {
    if (N <= 1024) return 100;
    if (N <= 8192) return 50;
    if (N <= 65536) return 20;
    return 10;
}

int compare_double(const void *a, const void *b) {
    double diff = *(const double *)a - *(const double *)b;
    return (diff > 0) - (diff < 0);
}

double median(double *arr, int len) {
    qsort(arr, len, sizeof(double), compare_double);
    return arr[len / 2];  // Предполагаем нечетное CIRCLE_IT
}

double measure_median_time(int *array, int N, int K, void (*fill)(int *, int)) {
    double times[CIRCLE_IT];
    for (int i = 0; i < CIRCLE_IT; ++i) {
        fill(array, N);
        warm_up(array, N);
        times[i] = full_pass(array, N, K);
    }
    return median(times, CIRCLE_IT);
}

void create_outfile() {
    srand((unsigned int)time(NULL));
    FILE *file = fopen("result.csv", "w");
    if (!file) {
        perror("fopen");
        return;
    }

    fprintf(file, "N,Forward,Backward,Random\n");

    int inc = 1;
    int border = 1;

    for (int N = MIN_ARRAY_SIZE; N <= MAX_ARRAY_SIZE; N += inc) {
        int *array = malloc(N * sizeof(int));
        if (!array) {
            fprintf(stderr, "Memory allocation failed for N=%d\n", N);
            break;
        }

        int K = get_optimal_K(N);

        double t_forward  = measure_median_time(array, N, K, fill_forward);
        double t_backward = measure_median_time(array, N, K, fill_backward);
        double t_random   = measure_median_time(array, N, K, fill_random);

        fprintf(file, "%d,%.2f,%.2f,%.2f\n", N, t_forward, t_backward, t_random);
        printf("N = %d done\n", N);

        free(array);

        if ((N * 4) / 1024 >= border) {
            inc <<= 1;
            border <<= 1;
        }
    }

    fclose(file);
}

int main(void) {
    create_outfile();
    return 0;
}
