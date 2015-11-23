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
			test_output_file_name=${input_file//input/test_output}
			hklc_output_file_path=$2"/"$hklc_output_file_name
			test_output_file_path=$2"/"$test_output_file_name
			startTime=$(date +%s)
		
			cd $3
			./hklc $hklc_input_file_path >> $hklc_output_file_path
			endTime=$(date +%s)
			echo $startTime >> $hklc_output_file_path
			echo $endTime >> $hklc_output_file_path

			startTime=$(date +%s)
			./hklctest $hklc_input_file_path >> $test_output_file_path
			endTime=$(date +%s)
			echo $startTime >> $test_output_file_path
			echo $endTime >> $test_output_file_path
		fi
		
	done
}

input_dir=/home/cheyulin/gitrepos/Open-List-Proportional-Representation/testinput 
output_dir=/home/cheyulin/gitrepos/Open-List-Proportional-Representation/testoutput
hklc_release_dir=/home/cheyulin/gitrepos/Open-List-Proportional-Representation/release

runHklc $input_dir $output_dir $hklc_release_dir
