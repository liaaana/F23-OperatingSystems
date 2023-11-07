#!/bin/bash

gcc pager.c -o pager
gcc mmu2.c -o mmu2

mkdir -p /tmp/ex2
replacement_algorithms=("aging")
for replacement_algorithm in "${replacement_algorithms[@]}"; do
  touch /tmp/ex2/pagetable
  ./pager 50 20 "$replacement_algorithm" &
  pid_pager=$!
  gnome-terminal -- bash -c "./mmu2 50 R7 W10 R29 W20 R4 R7 W44 R11 W3 R12 R42 W26 W22 R46 R49 W40 W49 W23 W5 R10 R44 W14 W5 W49 W6 R34 W5 R46 W11 W5 W41 R6 R34 R41 R28 R23 R12 R4 W26 R47 W36 R4 R30 W15 W1 W26 R34 W1 R49 R22 R2 R2 R45 W34 W41 W15 W19 R12 W26 W47 W2 W23 W33 W39 W21 W33 W23 W13 R39 W30 R24 R25 W13 W5 R39 W29 R7 W28 R38 R43 W33 W47 R23 W32 W15 W17 R21 R12 W26 R11 R39 R33 W17 R12 R38 W21 W36 W29 W8 W2 $pid_pager; exec bash"
  wait
  rm -f /tmp/ex2/pagetable
done

rm mmu2
rm pager
rm -r /tmp/ex2
