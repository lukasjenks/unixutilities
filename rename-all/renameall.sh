#!/usr/bin/bash

function renamefiles() {
	if [ $# -eq 2 ]; then
		cmd="find . -name $1"
		filestorename="$(eval $cmd)"
		if [ "$filestorename" == "" ]; then
			exit 1
		fi
		echo "$filestorename" | \
			while read line; do
				replacementpath="$(eval dirname $line)"
				replacementpath+="/$2"
				cmd="mv $line $replacementpath"
				eval "$cmd"
            done
    else
		cat >&2 <<EOF

usage: $(basename $0) original_file_name new_file_name
Find every file with the the given original file name in
this directory and all subdirectories and rename the file
with the given new file name.

EOF
    fi
}
		
renamefiles $@
