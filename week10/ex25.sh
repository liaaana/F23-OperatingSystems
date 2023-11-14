#!/bin/bash

echo -e "Creating an empty file ex5.txt..."
touch ex5.txt

echo -e "\nRemoving write permission for everybody..."
chmod a-w ex5.txt

echo -e "\nDisplaying permission values for ex5.txt:..."
ls -l ex5.txt

echo -e "\nGranting all permissions to owner and others (not group)..."
chmod u=rwx,o=rwx ex5.txt

echo -e "\nDisplaying permission values for ex5.txt:..."
ls -l ex5.txt

echo -e "\nMaking group permissions equal to user permissions..."
chmod g=u ex5.txt

echo -e "\nDisplaying permission values for ex5.txt:..."
ls -l ex5.txt

rm ex5.txt

# Explanation of permission values:
# 660 - read and write permissions for the owner, read permissions for the group and others.
# 775 - read, write, and execute permissions for the owner and group, and read and execute permissions for others.
# 777 - read, write, and execute permissions for the owner, group, and others.

