#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>

void *aggregate(void *base, size_t size, int n, void *initial_value, void *(*opr)(const void *, const void *)) {
    if (size == sizeof(double)) {
        double *res = (double *)malloc(sizeof(double));
        *res = *(double *)initial_value;
        for (int i = 0; i < n; i++) {
            res = opr(res, (double *)(base + i * size));
        }
        return res;
    } else {
        int *res = (int *)malloc(sizeof(int));
        *res = *(int *)initial_value;
        for (int i = 0; i < n; i++) {
            res = opr(res, (int *)(base + i * size));
        }
        return res;
    }
}

void *add_int(const void *a, const void *b) {
    int *res = (int *)malloc(sizeof(int));
    *res = (*(int *)a) + (*(int *)b);
    return res;
}

void *add_double(const void *a, const void *b) {
    double *res = (double *)malloc(sizeof(double));
    *res = (*(double *)a) + (*(double *)b);
    return res;
}

void *mul_int(const void *a, const void *b) {
    int *res = (int *)malloc(sizeof(int));
    *res = (*(int *)a) * (*(int *)b);
    return res;
}

void *mul_double(const void *a, const void *b) {
    double *res = (double *)malloc(sizeof(double));
    *res = (*(double *)a) * (*(double *)b);
    return res;
}

void *max_int(const void *a, const void *b) {
    int *res = (int *)malloc(sizeof(int));
    *res = (*(int *)a > *(int *)b) ? (*(int *)a) : (*(int *)b);
    return res;
}

void *max_double(const void *a, const void *b) {
    double *res = (double *)malloc(sizeof(double));
    *res = (*(double *)a > *(double *)b) ? (*(double *)a) : (*(double *)b);
    return res;
}



int main() {
    int p[5] = {5, 3, 18, 2, 43};
    double q[5] = {3.5, 5.5, 11.5, 4.5, 2.5};
    int n = 5;

    printf("Array p:\n");
    for (int i = 0; i < 5; i++) {
        printf("%d ", p[i]);
    }

    void* initial_value_int;
    int b = 0;
    initial_value_int = (void*)&b;
    int *result_int_add = (int *)aggregate(p, sizeof(p[0]), n, initial_value_int, add_int);
    printf("\nAddition result for int: %d\n", *result_int_add);
    free(result_int_add);

    b = 1;
    initial_value_int = (void*)&b;
    int *result_int_mul = (int *)aggregate(p, sizeof(p[0]), n, initial_value_int, mul_int);
    printf("Multiplication result for int: %d\n", *result_int_mul);
    free(result_int_mul);

    b = INT_MIN;
    initial_value_int = (void*)&b;
    int *result_int_max = (int *)aggregate(p, sizeof(p[0]), n, initial_value_int, max_int);
    printf("Max result for int: %d\n", *result_int_max);
    free(result_int_max);

    printf("\n");

    printf("Array q:\n");
    for (int i = 0; i < 5; i++) {
        printf("%.2lf ", q[i]);
    }
    void* initial_value_double;
    double bb = 0;
    initial_value_double = (void*)&bb;
    double *result_double_add = (double *)aggregate(q, sizeof(q[0]), n, initial_value_double, add_double);
    printf("\nAddition result for double: %.2lf\n", *result_double_add);
    free(result_double_add);

    bb = 1;
    initial_value_double = (void*)&bb;
    double *result_double_mul = (double *)aggregate(q, sizeof(q[0]), n, initial_value_double, mul_double);
    printf("Multiplication result for double: %.2lf\n", *result_double_mul);
    free(result_double_mul);

    bb = DBL_MIN;
    initial_value_double = (void*)&bb;
    double *result_double_max = (double *)aggregate(q, sizeof(q[0]), n, initial_value_double, max_double);
    printf("Max result for double: %.2lf\n", *result_double_max);
    free(result_double_max);

    return 0;
}

