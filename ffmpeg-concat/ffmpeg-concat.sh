#!/bin/bash

remove=false

while getopts ":i:o:r" opt; do
  case ${opt} in
    i )
      input_dir=$OPTARG
      ;;
    o )
      output_file=$OPTARG
      ;;
    r )
      remove=true
      ;;
    \? )
      echo "Invalid option: $OPTARG" 1>&2
      exit 1
      ;;
    : )
      echo "Invalid option: $OPTARG requires an argument" 1>&2
      exit 1
      ;;
  esac
done
shift $((OPTIND -1))

# change dir to the location of your files
cd "${input_dir}"

# count the number of .mp4 and .MP4 files in the dir
total_steps=$(ls -1 *.mp4 *.MP4 2>/dev/null | wc -l)

# the command whose progress we want to track
command="ffmpeg -f concat -safe 0 -i mylist.txt -c copy ${output_file}"

# this is the command that will run and pipe its output
${command} | 
(
    # initialize counter
    current_step=0
    
    # for each line output by the command...
    while IFS= read -r line; do
        # ...increment the counter,
        current_step=$((current_step + 1))
        
        # calculate progress as an integer,
        progress=$((current_step * 100 / total_steps))
        
        # print a carriage return (\r) to get the cursor to the start of the line
        echo -ne "\rProgress: [${progress}%] ["
        # print the progress bar
        for ((i=0; i<$progress; i+=10)); do echo -n "#"; done
        # print some spaces to ensure we overwrite the whole line
        for ((i=$progress; i<100; i+=10)); do echo -n " "; done
        echo -n "]"
    done
    # finally, print a newline to move to the next line after the progress bar
    echo
)

# remove original files if requested and if the previous command was successful
if [ "${remove}" = true ] && [ "$?" -eq 0 ]; then
  rm *.mp4 *.MP4
fi
