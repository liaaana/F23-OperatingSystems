#!/bin/bash

echo -e "\nWriting the hello world program ex1.c..."
echo '#include <stdio.h>

int main() {
    printf("Hello, World!\n");
    return 0;
}' > ex1.c

echo -e "\nCompiling ex1.c..."
gcc ex1.c -o ex1

echo -e "Check the inode of the program ex1"
inode_ex1=$(ls -i ex1 | cut -d ' ' -f 1)
echo "Inode of ex1: $inode_ex1"

echo -e "\nDetermine file attributes for ex1..."

echo "Number of blocks for ex1: $(stat -c "%b" ex1)"
echo "Size of each block for ex1: $(stat -c "%B" ex1) bytes"
echo "Total size of ex1: $(stat -c "%s" ex1) bytes"
echo "Permissions of ex1: $(stat -c "%A" ex1)"

echo -e "\nCopying ex1 to ex2"
cp ex1 ex2

echo -e "\nChecking number of links and inode numbers for ex2..."
links_ex2=$(stat -c "%h" ex2)
inode_ex2=$(ls -i ex2 | cut -d ' ' -f 1)

echo "Number of links for ex2: $links_ex2"
echo "Inode of ex2: $inode_ex2"

if [ "$inode_ex1" == "$inode_ex2" ]; then
    echo "Inode numbers for ex1 and ex2 are the same."
else
    echo "Inode numbers for ex1 and ex2 are different."
fi


echo -e "\nIdentifying files with 3 links in /etc..."
files_with_3_links=$(stat -c "%h - %n" /etc/* | grep '^3')
echo -e "Files with 3 links in /etc:\n$files_with_3_links"

rm ex1.c
rm ex1
rm ex2

# The inode numbers for ex1 and ex2 are distinct since we copied ex1 to ex2 without creating any kind of link, making them two entirely separate files.

# The number of links represents the hard link count for each file in /etc. A hard link is an additional reference to an existing inode. Multiple hard links pointing to the same inode share the same file content and attributes but have different filenames. These files may have multiple references in the file system, and any changes to one link affect the others.


