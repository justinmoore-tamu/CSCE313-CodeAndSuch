#!/bin/bash
rm timeLarge.txt
for ((i=200 ; i <= 10000 ; i = i+200));
do
	/usr/bin/time -f "\t%E real" -o timeLarge.txt -a ./client -f largeTest.bin -m $i
done
