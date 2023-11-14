#!/bin/bash

echo -e "\nChecking file types in /dev:"
file /dev/*

echo -e "\nChecking file types in /etc:"
file /etc/*

echo -e "\nCounting the number of directories in /etc:"
ls -l /etc | grep ^d | wc -l

echo -e "\nWriting the hello world program ex1.c..."
echo '#include <stdio.h>

int main() {
    printf("Hello, World!\n");
    return 0;
}' > ex1.c

echo -e "\nChecking file type of ex1.c before compilation:"
file ex1.c

echo -e "\nCompiling ex1.c..."
gcc ex1.c -o ex1

echo -e "\nChecking file type of ex1 after compilation:"
file ex1

echo -e "\nModifying the program to print Привет, мир! instead of Hello, World!..."
echo '#include <stdio.h>

int main() {
    printf("Привет, мир!\n");
    return 0;
}' > ex1.c

echo -e "Recompiling ex1.c..."
gcc ex1.c -o ex1

echo -e "\nChecking file type of ex1.c after modification and recompilation:"
file ex1.c

rm ex1.c
rm ex1

# Difference: The difference observed in file types before and after modification is due to the change in the content of ex1.c. Before modification, the file type is C source, ASCII text. After modification, the file type is C source, Unicode text, reflecting the inclusion of non-ASCII characters. This change indicates the use of Unicode (UTF-8) encoding to support russian characters in the source code.

