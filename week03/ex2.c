#include <stdio.h>
#include <math.h>

struct Point {
    double x;
    double y;
};

double distance(struct Point point1, struct Point point2) {
    return sqrt(pow((point2.x - point1.x), 2) + pow((point2.y - point1.y), 2));
}

double area(struct Point point1, struct Point point2, struct Point point3) {
    return 0.5 * fabs(point1.x * (point2.y - point3.y) + point2.x * (point3.y - point1.y) + point3.x * (point1.y - point2.y));
}


int main() {
    struct Point A = {.x=2.5, .y=6};
    struct Point B = {.x=1, .y=2.2};
    struct Point C = {.x=10, .y=6};

    printf("Point A: (%lf, %lf)\n", A.x, A.y);
    printf("Point B: (%lf, %lf)\n", B.x, B.y);
    printf("Point C: (%lf, %lf)\n", C.x, C.y);
    printf("\n");
    printf("Distance between point A and point B: %lf\n", distance(A, B));
    printf("Area of the triangle ABC: %lf\n", area(A, B, C));
    return 0;
}
