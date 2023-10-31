#!/bin/bash

pid=$(cat /tmp/ex1.pid)
address_heap=$(sudo grep "heap" /proc/"$pid"/maps | cut -d " " -f 1)
info=$(sudo xxd -s 0x$(echo "$address_heap" | cut -d "-" -f 1) -l $((0x$(echo "$address_heap" | cut -d "-" -f 2) - 0x$(echo "$address_heap" | cut -d "-" -f 1))) /proc/"$pid"/mem | grep "pass:")

if [ -z "$info" ]; then
  echo "Password address not found"
  exit 1
fi

p=$(echo "$info" | grep -oP '(?<=pass:).{8}')
p_address=$(echo "$info" | cut -d ":" -f 1)

if [ -z "$p" ]; then
  echo "Password not found"
else
  echo "Password: $p"
  echo "Password address: 0x$p_address"
fi

sudo kill -SIGKILL "$pid"
