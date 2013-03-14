#!/bin/bash

gcc -o ag ag_bin.c -lm
gcc -o ag_ms  ag_bin_ms.c  -fopenmp -lm
gcc -o ag_md  ag-mult-deme.c -lm -fopenmp
gcc -o ag_msmd ag-mult-deme-ms.c -fopenmp -lm

for (( i = 0; i < 11; i++ )); do
	echo $i 
	(time ./ag  1> run$i) 2>> run$i
	echo "ag done"
	(time ./ag_ms  1> run-ms$i) 2>> run-ms$i
	echo "ms done"
	(time ./ag_md  1> run-md$i) 2>> run-md$i
	echo "md done"
	(time ./ag_msmd  1> run-msmd$i) 2>> run-msmd$i
	echo "done"
done