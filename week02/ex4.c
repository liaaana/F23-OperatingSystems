#include <stdio.h>
#include <string.h>
#include <ctype.h>

int count(const char *givenString, char charToCount) {
    int count = 0;
    int i = 0;
    while (givenString[i] != '\0') {
        if (tolower(givenString[i]) == tolower(charToCount)) {
            count++;
        }
        i++;
    }
    return count;
}

void countAll(const char *s) {
    for (int i = 0; i < strlen(s)-1; i++) {
        printf("%c:%d", tolower(s[i]), count(s, s[i]));
        if (i != strlen(s)-2){
            printf(", ");
        }
    }
    printf("\n");
}

int main() {
    char strToCount[257];
    printf("Enter a string no longer than 256 characters: ");
    fgets(strToCount, sizeof(strToCount), stdin);
    countAll(strToCount);

    return 0;
}
