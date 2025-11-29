#include <cblas.h>
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

void transpose(Matrix* matrix) {
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

void make_I_matrix(Matrix* matrix) {
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

void matrix_copy(Matrix* dest, const Matrix* src) {
    const size_t n = dest->size;
    for (size_t i = 0; i < n * n; ++i) {
        dest->elements[i] = src->elements[i];
    }
}

void matrix_sub(Matrix* matrix, const Matrix* term) {
    const size_t n = matrix->size;
    cblas_saxpy(n * n, -1.0f, term->elements, 1, matrix->elements, 1);
}

void matrix_add(Matrix* matrix, const Matrix* term) {
    const size_t n = matrix->size;
    cblas_saxpy(n * n, 1.0f, term->elements, 1, matrix->elements, 1);
}

void matrix_mul(const Matrix* matrix, const Matrix* other, Matrix* result) {
    const size_t n = matrix->size;
    cblas_sgemm(
        CblasRowMajor,
        CblasNoTrans,
        CblasTrans,
        n,
        n,
        n,
        1.0f,
        matrix->elements,
        n,
        other->elements,
        n,
        0.0f,
        result->elements,
        n
    );
}

void mul_by_scalar(Matrix* matrix, float scalar) {
    const size_t n = matrix->size;
    cblas_sscal(n * n, scalar, matrix->elements, 1);
}

void mul_by_matrix(Matrix* matrix, const Matrix* other, Matrix* tmp) {
    matrix_mul(matrix, other, tmp);
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

    matrix_copy(B, matrix);
    transpose(B);
    float A_inf = calc_form_inf(matrix);
    float A_1 = calc_form_1(matrix);
    mul_by_scalar(B, 1.0f / A_inf);
    mul_by_scalar(B, 1.0f / A_1);

    make_I_matrix(R);
    matrix_copy(matrix_transposed, matrix);
    transpose(matrix_transposed);
    matrix_mul(B, matrix_transposed, BA);
    matrix_sub(R, BA);

    make_I_matrix(R_power);
    transpose(R);
    make_I_matrix(result);

    for (size_t i = 1; i < m; ++i) {
        matrix_mul(R_power, R, tmp);
        matrix_copy(R_power, tmp);
        matrix_add(result, R_power);
    }

    transpose(B);
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

//Simple test 4x4
/*void test_4x4() {
    Matrix* A = create_matrix(4);
    A->elements[0] = 2.0f; A->elements[1] = 0.0f;
    A->elements[2] = 0.0f; A->elements[3] = 0.0f;
    A->elements[4] = 0.0f; A->elements[5] = 3.0f;
    A->elements[6] = 0.0f; A->elements[7] = 0.0f;
    A->elements[8] = 0.0f; A->elements[9] = 0.0f;
    A->elements[10] = 4.0f; A->elements[11] = 0.0f;
    A->elements[12] = 0.0f; A->elements[13] = 0.0f;
    A->elements[14] = 0.0f; A->elements[15] = 5.0f;
    printf("Matrix:\n");
    print_matrix(A);
    printf("Expected matrix:\n");
    printf(" 0.500  0.000  0.000  0.000\n");
    printf(" 0.000  0.333  0.000  0.000\n");
    printf(" 0.000  0.000  0.250  0.000\n");
    printf(" 0.000  0.000  0.000  0.200\n\n");
    printf("M = 1000:\n");
    Matrix* inv1 = create_matrix(4);
    inverse_matrix(A, 1000, inv1);
    printf("Computed inverse:\n");
    print_matrix(inv1);
    printf("M = 10000:\n");
    Matrix* inv2 = create_matrix(4);
    inverse_matrix(A, 10000, inv2);
    printf("Computed inverse:\n");
    print_matrix(inv2);
    destroy_matrix(inv1);
    destroy_matrix(inv2);
    destroy_matrix(A);
}*/

int main(void) {
    //test_4x4();
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
