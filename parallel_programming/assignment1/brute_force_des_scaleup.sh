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
# key size
for(( k=10; k<=$2; k++ ));
do
	# iteration counts
	for(( i=1; i<=100; i++ ));
	do
		java BruteForcedDES $1 $k $i $3
	done
done
