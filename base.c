
#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    float* elements;
    size_t size;
} Matrix;

Matrix* create_matrix(size_t n) {
    Matrix* new_matrix = malloc(sizeof(Matrix));
    new_matrix->elements = calloc(n * n, sizeof(float));
    new_matrix->size = n;
    if (!new_matrix->elements) {
        free(new_matrix);
        return NULL;
    }
    return new_matrix;
}

void destroy_matrix(Matrix* matrix) {
    if (matrix) {
        free(matrix->elements);
        free(matrix);
    }
}

void fill_matrix(Matrix* matrix) {
    const size_t n = matrix->size;
    for (size_t i = 0; i < n * n; ++i) {
        matrix->elements[i] = (float)rand() / (float)RAND_MAX;
    }
}

void transpose_matrix(Matrix* matrix) {
    const size_t n = matrix->size;
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i; j < n; ++j) {
            float* a = &matrix->elements[i * n + j];
            float* b = &matrix->elements[j * n + i];
            float tmp = *a;
            *a = *b;
            *b = tmp;
        }
    }
}

void make_identity_matrix(Matrix* matrix) {
    const size_t n = matrix->size;
    for (size_t i = 0; i < n * n; ++i) {
        matrix->elements[i] = 0.0f;
    }
    for (size_t i = 0; i < n; ++i) {
        matrix->elements[i * n + i] = 1.0f;
    }
}

float calc_form_1(const Matrix* matrix) {
    const size_t n = matrix->size;
    float result = 0.0f;
    for (size_t i = 0; i < n; ++i) {
        const float* current_row = matrix->elements + i * n;
        float cur_sum = 0.0f;
        for (size_t j = 0; j < n; ++j) {
            cur_sum += fabsf(current_row[j]);
        }
        if (cur_sum > result) {
            result = cur_sum;
        }
    }
    return result;
}

float calc_form_inf(const Matrix* matrix) {
    const size_t n = matrix->size;
    float result = 0.0f;
    for (size_t i = 0; i < n; ++i) {
        float cur_sum = 0.0f;
        for (size_t j = 0; j < n; ++j) {
            cur_sum += fabsf(matrix->elements[j * n + i]);
        }
        if (cur_sum > result) {
            result = cur_sum;
        }
    }
    return result;
}

float dot(const float* a, const float* b, size_t n) {
    float result = 0.0f;
    for (size_t k = 0; k < n; ++k) {
        result += a[k] * b[k];
    }
    return result;
}

void matrix_copy(Matrix* dest, const Matrix* src) {
    const size_t n = dest->size;
    for (size_t i = 0; i < n * n; ++i) {
        dest->elements[i] = src->elements[i];
    }
}

void matrix_sub(Matrix* matrix, const Matrix* term) {
    const size_t n = matrix->size;
    for (size_t i = 0; i < n * n; ++i) {
        matrix->elements[i] -= term->elements[i];
    }
}

void matrix_add(Matrix* matrix, const Matrix* term) {
    const size_t n = matrix->size;
    for (size_t i = 0; i < n * n; ++i) {
        matrix->elements[i] += term->elements[i];
    }
}

void matrix_mul(const Matrix* matrix, const Matrix* transposed, Matrix* result) {
    const size_t n = matrix->size;
    for (size_t i = 0; i < n; ++i) {
        const float* row1 = matrix->elements + i * n;
        for (size_t j = 0; j < n; ++j) {
            const float* row2 = transposed->elements + j * n;
            result->elements[i * n + j] = dot(row1, row2, n);
        }
    }
}

void mul_by_scalar(Matrix* matrix, float scalar) {
    const size_t n = matrix->size;
    for (size_t i = 0; i < n * n; ++i) {
        matrix->elements[i] *= scalar;
    }
}

void mul_by_matrix(Matrix* matrix, const Matrix* transposed, Matrix* tmp) {
    matrix_mul(matrix, transposed, tmp);
    matrix_copy(matrix, tmp);
}

void inverse_matrix(const Matrix* matrix, size_t m, Matrix* result) {
    const size_t n = matrix->size;
    Matrix* B = create_matrix(n);
    Matrix* R = create_matrix(n);
    Matrix* matrix_transposed = create_matrix(n);
    Matrix* BA = create_matrix(n);
    Matrix* R_power = create_matrix(n);
    Matrix* tmp = create_matrix(n);

    // B = scalar * A^(T)
    matrix_copy(B, matrix);
    transpose_matrix(B);
    float A_inf = calc_form_inf(matrix);
    float A_1 = calc_form_1(matrix);
    mul_by_scalar(B, 1.0f / A_inf);
    mul_by_scalar(B, 1.0f / A_1);

    // R = I - BA
    make_identity_matrix(R);
    matrix_copy(matrix_transposed, matrix);
    transpose_matrix(matrix_transposed);
    matrix_mul(B, matrix_transposed, BA);
    matrix_sub(R, BA);

    make_identity_matrix(R_power);
    transpose_matrix(R);
    make_identity_matrix(result);

    for (size_t i = 1; i < m; ++i) {
        matrix_mul(R_power, R, tmp);
        matrix_copy(R_power, tmp);
        matrix_add(result, R_power);
    }

    transpose_matrix(B);
    mul_by_matrix(result, B, tmp);

    destroy_matrix(B);
    destroy_matrix(R);
    destroy_matrix(matrix_transposed);
    destroy_matrix(BA);
    destroy_matrix(R_power);
    destroy_matrix(tmp);
}

void print_matrix(const Matrix* matrix) {
    const size_t n = matrix->size;
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            printf("%6.3f ", matrix->elements[n * i + j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Test for 3x3 matrix
void test_3x3() {
    Matrix* A = create_matrix(3);
    Matrix* inv1 = create_matrix(3);
    Matrix* inv2 = create_matrix(3);

    A->elements[0] = 2.0f; A->elements[1] = 0.0f; A->elements[2] = 1.0f;
    A->elements[3] = 0.0f; A->elements[4] = 1.0f; A->elements[5] = 2.0f;
    A->elements[6] = 1.0f; A->elements[7] = 0.0f; A->elements[8] = 1.0f;

    printf("Matrix A:\n");
    print_matrix(A);
    printf("Expected inverse matrix:\n");
    printf("  1.000  0.000 -1.000\n");
    printf("  2.000  1.000 -4.000\n");
    printf(" -1.000  0.000  2.000\n\n");

    printf("M = 1000:\n");
    inverse_matrix(A, 1000, inv1);
    printf("Computed inverse:\n");
    print_matrix(inv1);

    printf("M = 10000:\n");
    inverse_matrix(A, 10000, inv2);
    printf("Computed inverse:\n");
    print_matrix(inv2);

    destroy_matrix(A);
    destroy_matrix(inv1);
    destroy_matrix(inv2);
}

int main(void) {
    //test_3x3();
    srand((unsigned int)time(NULL));
    Matrix* matrix = create_matrix(2048);
    Matrix* result = create_matrix(2048);

    fill_matrix(matrix);

    time_t t0;
    time(&t0);

    inverse_matrix(matrix, 10, result);

    time_t t1;
    time(&t1);

    printf("Time taken: %ld s\n", t1 - t0);

    destroy_matrix(matrix);
    destroy_matrix(result);
    return 0;
}
