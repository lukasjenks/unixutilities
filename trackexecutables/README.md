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
