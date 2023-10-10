#!/bin/bash

if [ "$#" -ne 1 ]; then
  echo "Give number of subscribers as an argument"
  exit 1
fi

n=$1
if [ $n -lt 1 ] || [ $n -gt 3 ]; then
  echo "Number of subscribers should be in [1:3]"
  exit 1
fi

mkdir -p /tmp/ex1

gcc publisher.c -o publisher
gcc subscriber.c -o subscriber
for ((i = 1; i <= $n; i++)); do
  gnome-terminal -- bash -c "./subscriber $i; exec bash" &
done
./publisher $n

wait
rm publisher
rm subscriber

