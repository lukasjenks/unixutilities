# Unix Utilities

## cpu-users

**Description**

A bash script which, for the UID of each user running a process on the Unix system it was executed on, sums the CPU time (user & system) of all the user's processes.

**Usage**

`cpuusers`

## custom-shell

**Description**

This is a very rough draft of a proof of concept for a custom unix shell. It is by no means finished/refined and hasn't been worked on for some time.

**Compilation Instructions**

`make all`
- This will trigger the target `make shell`

`make shell`
- This will compile shell.c into the executable shell

**Usage**

`shell`
- This will execute and initialize the interactive shell

## find-user

**Description**

Bash script which finds every user in a Unix system that has all of the search terms present in their account's comment field.

**Usage**

`finduser [search_terms_here]`

## next-group

**Description**

Determines the minimum unused GID in the Unix system. If provided, arguments n and/or m limit (inclusively) the minimum and maximuim GIDs for the search.

**Usage**

`nextgroup [options] [[n]-[m]]`

options: -h Displays a help message including usage and description.

## rename-all

**Description**

This is a simple bash script that finds every file with the the given original file name in the pwd and all subdirectories and renames the files with the given new file name.

**Usage**

`rename-all [orig_file_name] [new_file_name]`

## track-executables

**Description**

This program scans the processes under /proc and determines if it has any W&X memory. The output of the program is a list of the process IDs and executable names for the process.

`wx_scan`
- This will list all processes currently running with W&X memory segments

`wx_scan -k`
- This will list and kill all processes currently running with W&X memory segments

The second program, setwx, is for testing. Executing this program will initialize a new process with W&X memory, and will then sleep for a brief period. See below "Testing Instructions" for usage.

**Compiling Instructions:**

`make wx_scan`
- This will compile wx_scan.c into the executable wx_scan

`make setwx`
- This will compile setwx.c into the executable setwx

`make all`
- This will compile both wx_scan.c and setwx.c to the executables wx_scan and setwx, respectively.

`make test`
- This will execute setwx 2 times, then wx_scan once, then execute
  setwx 4 times, then wx_scan once, then execute setwx 6 times, then
  wx_scan once.

`make clean`
- This will cleanup all executable and residual files

**Testing Instructions**
To test, run:

`make all`

`make test`

**Notes**
To determine how to complete this project, I first examined the proc directory and its contents, and researched the properties of the 
proc directory in the linux filesystem (see reference in wx_scan.c). What I found was that within /proc, the subdirectories 
which had names of a series of digits corresponded to a process, with the name being the process id. Also, I took note of 
some files that I knew I had to get information from:

/proc/[pid]/maps <--(contains memory maps which lists the permissions for the processes memory segments)

/proc/[pid]/status <--(contains the executable name on the first line)

Using this information, I took the approach of isolating the subdirectories within proc that corresponded to a process, 
(subdirectories with any number of digits as its name), searching through each processes maps file line by line, looking
for memory segments with the w&x permissions set. After isolating processes with the w&x permissions set somewhere within
its maps file, I implemented extracting the executable name from the status file and outputting the pid and executable 
name, additionally killing said processes if the "-k" command line argument was given.

**Small Note:** 
Though checkpatch produces one warning, saying "struct should normally be const" for line 122 of wx_scan.c, changing
the struct to a const struct produces compiler warnings, so I left the struct on line 122 of wx_scan.c as a regular struct.
