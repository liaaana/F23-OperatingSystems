#!/bin/bash

mkdir -p /tmp/ex2
touch /tmp/ex2/pagetable
gcc mmu.c -o mmu
gcc pager.c -o pager

./pager 4 2 &
pid_pager=$!
gnome-terminal -- bash -c "./mmu 4 R0 R1 W1 R0 R2 W2 R0 R3 W2 $pid_pager; exec bash"
wait


rm mmu
rm pager
rm -r /tmp/ex2
