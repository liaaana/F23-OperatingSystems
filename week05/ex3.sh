#!/bin/bash
gcc -pthread ex3.c -o ex3
number_of_threads=(1 2 4 10 100)
echo "Execution times for n = 10,000,000" >ex3_res.txt
echo "---------------------------------" >>ex3_res.txt
for m in "${number_of_threads[@]}"; do
  {
    echo -e "\nNumber of threads m = $m"
    echo "---------------------"
  } >>ex3_res.txt
  { time ./ex3 10000000 "$m"; } 2>>ex3_res.txt
done
rm ex3
