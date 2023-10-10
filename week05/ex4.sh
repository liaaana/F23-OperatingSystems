#!/bin/bash
gcc -pthread ex4.c -o ex4
number_of_threads=(1 2 4 10 100)
echo "Execution times for n = 10,000,000" >ex4_res.txt
echo "---------------------------------" >>ex4_res.txt
for m in "${number_of_threads[@]}"; do
  {
    echo -e "\nNumber of threads m = $m"
    echo "---------------------"
  } >>ex4_res.txt
  { time ./ex4 10000000 "$m"; } 2>>ex4_res.txt
done
rm ex4
