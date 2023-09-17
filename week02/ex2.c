#include <stdio.h>

int main() {
    char givenString[257]; // 256 characters + null terminator
    char resultString[257]; // 256 characters + null terminator
    int givenStringLength = 0;
    char inputChar;

    printf("Enter a string no longer than 256 characters: ");

    while (givenStringLength < 256) {
        scanf("%c", &inputChar);
        if (inputChar == '.' || inputChar == '\n') {
            break;
        }
        givenString[givenStringLength] = inputChar;
        givenStringLength++;
    }
    givenString[givenStringLength] = '\0';

    int resultStringLength = givenStringLength;
    for (int i = 0; i <= resultStringLength; i++) {
        if (i == resultStringLength) {
            resultString[i] = '\0';
        } else {
            resultString[i] = givenString[givenStringLength - i - 1];
        }
    }
    printf("Result: \"%s\"\n", resultString);
    return 0;
}
