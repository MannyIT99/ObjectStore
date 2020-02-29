#!/bin/bash

while IFS= read -r line
do
	./client $line 1 < trystore.txt &
done < nomi.txt
while IFS= read -r line
do
    ./client $line 2 < tryret.txt &
done < nomi2.txt
while IFS= read -r line
do
    ./client $line 3 < trydelete.txt &
done < nomi3.txt
