#!/bin/bash

echo -e "Deleting ./tmp folder if it exists and creating a symbolic link tmp1..."
if [ -d ./tmp ]; then
    rm -r ./tmp
fi
ln -s "$(pwd)/tmp" tmp1

echo -e "\nRunning ls -li..."
ls -li

echo -e "\nCreating the folder ./tmp..."
mkdir ./tmp

echo -e "\nRunning ls -li to check the difference..."
ls -li

echo -e "\nCreating ex1.txt using gen.sh and adding it to ./tmp and chkeing the folder ./tmp1..."
./gen.sh 10 ex1.txt
mv ex1.txt ./tmp
cd ~/week10/tmp1
ls -li
cd ~/week10

echo -e "\nCreating another symbolic link tmp1/tmp2 to ./tmp..."
ln -s "$(pwd)/tmp" "$(pwd)/tmp1/tmp2"

echo -e "\nCreating ex1.txt using gen.sh and adding it to ./tmp1/tmp2..."
./gen.sh 10 ex1.txt
mv ex1.txt ./tmp1/tmp2

echo -e "\nChecking the content of the sub-folders..."
echo -e "\nls -li for tmp2:"
cd ~/week10/tmp1/tmp2
ls -li
cd ~/week10
echo -e "\nls -R for tmp1:"
ls -R ~/week10/tmp1

echo -e "\nTrying to access the sub-folders..."
cd ~/week10/tmp1/tmp2/tmp2/tmp2
cd ~/week10

echo -e "\nDeleting the folder ./tmp and checking the symbolic links again..."
rm -r ~/week10/tmp
ls -li

echo -e "\nDeleting all other symbolic links..."
rm -r ~/week10/tmp1

# Explanation for difference of two runnings of ls -li: Before there is a symbolic link tmp1 pointing to a non-existent ./tmp folder. After, the ./tmp folder is created, we see a new entry in the ls -li output --> entry for tmp (it has already 2 hlinks). The inode number for tmp1 remains the same, indicating that tmp1 is a symbolic link.


# Explanation: The symbolic link tmp1/tmp2 is created as a relative link to ./tmp, creating a recursive link. Accessing the sub-folders leads to an infinite loop of symbolic links, potentially causing a stack overflow.

