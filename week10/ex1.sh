#!/bin/bash

dir=$(pwd)
mkdir my_dir
gcc monitor.c -o monitor 
gcc ex1.c -o ex1 
./monitor "$dir/my_dir" &
sleep 1
gnome-terminal -- bash -c "./ex1 $dir/my_dir; exec bash"
cd "$dir/my_dir"
bash "$dir/ex1_test.sh"
cd "$dir"
pkill -INT monitor
sleep 1
rm monitor 
rm ex1
rm -r my_dir
