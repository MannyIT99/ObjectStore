#!/bin/bash
cl=0
ca=0
t1=0
t2=0
t3=0
while IFS= read -r line
do
    if [[ $line == *"fallite"* ]]; then
        ((cl++))
        if [[ ! $line == *"Numero di operazioni fallite: 0" ]]; then
            ((ca++))
            if [[ $line == *"TEST1"* ]]; then
                templine=${line#"[ TEST1 ] Numero di operazioni fallite: "}
                t1=$((t1+templine))
            elif [[ $line == *"TEST2"* ]]; then
                templine=${line#"[ TEST2 ] Numero di operazioni fallite: "}
                t2=$((t2+templine))
            elif [[ $line == *"TEST3"* ]]; then
                templine=${line#"[ TEST3 ] Numero di operazioni fallite: "}
                t3=$((t3+templine))
            else
                continue
            fi
        fi
    fi
done < testout.log
echo -e "\n*** RISULTATI TESTSUM ***"
echo "Numero di client lanciati: ${cl}"
echo "Numero di client che hanno riportato anomalie: ${ca}"
echo "Numero di anomalie per batteria di tipo #1: ${t1}"
echo "Numero di anomalie per batteria di tipo #2: ${t2}"
echo -e "Numero di anomalie per batteria di tipo #3: ${t3}\n"
killall -SIGUSR1 server

