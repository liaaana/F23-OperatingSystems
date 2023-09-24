#include <stdio.h>
#include <stdlib.h>

long long const_tri(int *const pp, int nn) {
    for (int i = 3; i <= nn; i++) {
        pp[i % 3] = pp[0] + pp[1] + pp[2];
    }
    return pp[nn % 3];
}

int main() {
    int n;
    printf("Enter N for finding N-th Tribonacci number: \n");
    scanf("%d", &n);

    const int x = 1;
    const int *q = &x;
    int *const p = malloc(3 * sizeof(int));
    p[0] = *q;
    p[1] = *q;
    p[2] = 2 * *q;

    printf("\nMemory addresses of cells for finding Tribonacci number: %p %p %p \n", p, p + 1, p + 2);
    printf("Contiguous: ");
    // casting pointers to char *, to work with byte-level differences
    if ((char *) (p + 1) - (char *) p == sizeof(int) && (char *) (p + 2) - (char *) (p + 1) == sizeof(int)) {
        printf("Yes\n");
    } else {
        printf("No\n");
    }

    if (n >= 0) {
        printf("%d-th Tribonacci number: %lld \n", n, const_tri(p, n));
    } else {
        printf("Impossible to count %d-th Tribonacci number, n should be >= 0 \n", n);
    }
    free(p);
    return 0;
}


