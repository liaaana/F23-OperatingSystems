#include "stdio.h"

void convert(long long x, int s, int t) {
    if (s < 2 || t < 2 || s > 10 || t > 10) {
        printf("cannot convert!\n");
        return;
    }

    long long decimalNumber = 0;
    int base = 1;
    int n;

    // x to decimal from source number system
    while (x != 0) {
        n = x % 10;
        if (n >= s) {
            printf("cannot convert!\n");
            return;
        }
        x = x / 10;
        decimalNumber += n * base;
        base *= s;
    }

    int convertedNumber[65];
    int digitIndex = 0;

    // decimalNumber to the target number system
    while (decimalNumber != 0) {
        n = decimalNumber % t;
        convertedNumber[digitIndex] = n;
        decimalNumber = decimalNumber / t;
        digitIndex++;
    }

    printf("Result: ");
    for (int j = digitIndex - 1; j >= 0; j--) {
        printf("%d", convertedNumber[j]);
    }
    printf("\n");
}


int main(){
    long long inputNumber;
    int sourceNumberSystem;
    int targetNumberSystem;

    printf("Enter number to convert:\n");
    scanf("%lld", &inputNumber);
    printf("Enter source number system:\n");
    scanf("%d", &sourceNumberSystem);
    printf("Enter target number system:\n");
    scanf("%d", &targetNumberSystem);

    convert(inputNumber, sourceNumberSystem, targetNumberSystem);
    return 0;
}
