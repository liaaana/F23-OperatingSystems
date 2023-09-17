#include <stdio.h>
#include <limits.h>
#include <float.h>

int main() {
    int v1 = INT_MAX;
    printf("Type: int\n");
    printf("Size of int: %lu bytes\n", sizeof(v1));
    printf("Maximum value of int: %d\n\n", v1);

    unsigned short v2 = USHRT_MAX;
    printf("Type: unsigned short int\n");
    printf("Size of unsigned short int: %lu bytes\n", sizeof(v2));
    printf("Maximum value of unsigned short int: %hu\n\n", v2);

    long int v3 = LONG_MAX;
    printf("Type: long int\n");
    printf("Size of long int: %lu bytes\n", sizeof(v3));
    printf("Maximum value of long int: %ld\n\n", v3);

    float v4 = FLT_MAX;
    printf("Type: float\n");
    printf("Size of float: %lu bytes\n", sizeof(v4));
    printf("Maximum value of float: %f\n\n", v4);

    double v5 = DBL_MAX;
    printf("Type: double\n");
    printf("Size of double: %lu bytes\n", sizeof(v5));
    printf("Maximum value of double: %lf\n\n", v5);

    return 0;
}
