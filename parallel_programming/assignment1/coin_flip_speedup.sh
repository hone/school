#!/bin/bash

#Merge the standard output and standard error
#$ -j y

#Execute the job from the current working directory
#$ -cwd

#Specify that the interpreting shell for this job is the Bash shell
#$ -S /bin/bash

#Execute the initialization script
. $HOME/.bashrc

#echo "#Iteration	Threads	ElapsedTime"
for(( k=1; k<=$2; k++ ));
do
	for(( j=1; j<=$1; j++ ));
	do
        for(( i=1; i<=20; i++ ));
		do
			let ITERATIONS=10000000*k
			java CoinFlip $j $ITERATIONS $i
		done
	done
done
