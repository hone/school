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
# iteration_max_multiplier
for(( j=1; j<=$2; j++ ));
do
	# iteration counts
	for(( i=1; i<=20; i++ ));
	do
		let ITERATIONS=10000000*j
		java CoinFlip $1 $ITERATIONS $i $3
	done
done
