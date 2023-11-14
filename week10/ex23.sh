#!/bin/bash

echo -e "Create ex1.txt with 10 random numbers by using gen.sh..."
./gen.sh 10 ex1.txt

echo -e "\nLinking ex1.txt to ex11.txt and ex12.txt..."
ln ex1.txt ex11.txt
ln ex1.txt ex12.txt

echo -e "\nComparing the content of all files using cat:..."
cat_result1=$(cat "$(pwd)/ex1.txt" )
cat_result11=$(cat "$(pwd)/ex11.txt" )
cat_result12=$(cat "$(pwd)/ex12.txt" )

if [ "$cat_result1" == "$cat_result11" ]; then
    echo "Files ex1.txt and ex11.txt have the same content."
else
    echo "Files ex1.txt and ex11.txt have different content."
fi

if [ "$cat_result1" == "$cat_result12" ]; then
    echo "Files ex1.txt and ex12.txt have the same content."
else
    echo "Files ex1.txt and ex12.txt have different content."
fi

echo -e "\nChecking i-node numbers of all files and saving to output.txt:..."
ls -i ex1.txt ex11.txt ex12.txt > output.txt

echo -e "\nContent of output.txt..."
cat output.txt
echo -e "We see that i-nodes are the same."

echo -e "\nChecking the disk usage of ex1.txt using du command:..."
du ex1.txt

echo -e "\nCreating a hard link ex13.txt and moving it to /tmp..."
ln ex1.txt ex13.txt
mv ex13.txt /tmp

echo -e "\nTracing all links to ex1.txt in the current path..."
find . -inum $(stat -c "%i" ex1.txt)

echo -e "\nTracing all links to ex1.txt in the root path (/)..."
find / -inum $(stat -c "%i" ex1.txt) 2>/dev/null

echo -e "\nChecking the number of hard links of ex1.txt..."
ls -l ex1.txt

echo -e "\nRemoving all links from ex1.txt..."
find . -inum $(stat -c "%i" ex1.txt) -exec rm {} \;

rm output.txt

# Justification for content comparison: The diff command does not show any differences because contents of all linked files are the same.

# Justification for i-node numbers: The i-node numbers are the same for ex1.txt, ex11.txt, and ex12.txt because they are hard links to the same i-node.

# Difference between searching in the current path and the root path: When searching in the current path (.), it will only find links to ex1.txt in the current directory and its subdirectories. When searching in the root path (/), it will find links to ex1.txt across the entire filesystem, starting from the root directory.

