#!/bin/bash

#Merge the standard output and standard error
#$ -j y

#Execute the job from the current working directory
#$ -cwd

#Specify that the interpreting shell for this job is the Bash shell
#$ -S /bin/bash

#Execute the initialization script
. $HOME/.bashrc

for(( i=0; i<=20; i++ ));
do
	for(( j=1; j<=$1; j++ ));
	do
		for(( k=1; k<=$2; k++ ));
		do
			let ITERATIONS=10000000*k
			echo "Executing CoinFlip with $k thread for $ITERATIONS coin flips"
			java CoinFlip $k $ITERATIONS
		done
	done
done
