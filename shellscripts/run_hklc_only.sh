#!/bin/bash
#parameter1:input dir, parameter2:ouput dir parameter3:hklc dir
function runHklc(){
	#create output dir
	if [ ! -d $2 ]; then  
	mkdir $2
	fi  
	for existing_file in $(ls $2)
	do
		rm $2"/"$existing_file
	done

	for input_file in $(ls $1)
	do
		if [[ $input_file =~ ^input.*$ ]];then
			hklc_input_file_path=$1"/"$input_file
			hklc_output_file_name=${input_file//input/hklc_output}
			hklc_output_file_path=$2"/"$hklc_output_file_name
			startTime=$(date +%s)
			cd $3
			./hklc $hklc_input_file_path >> $hklc_output_file_path
			endTime=$(date +%s)
			echo $startTime >> $hklc_output_file_path
			echo $endTime >> $hklc_output_file_path
		fi
		
	done
}

input_dir=/home/cheyulin/Gitrepos/Open-List-Proportional-Representation/testinputcyl
output_dir=/home/cheyulin/Gitrepos/Open-List-Proportional-Representation/testoutputcyl
hklc_release_dir=/home/cheyulin/Gitrepos/Open-List-Proportional-Representation/release

runHklc $input_dir $output_dir $hklc_release_dir
