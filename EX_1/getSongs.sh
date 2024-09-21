#!/bin/bash

# searches for a given string in text file and returns the number of lines the string occures.

# by Tinsae Ghilay Kahsay on september 5, 2024

# check if param passed to command
if [[ $# -eq "0" ]] ; then
    # no param passed?
    echo "Error: use the script with a parameter. Check the error log for details."
    echo "$(date) :: Error: invalid input. Script expects an input parameter of type string!." >> errorLog.txt

else
    count=$(grep -ic $1  songs.txt)

    if [[ $count -gt 0 ]] ; then 
        echo "$count hits found for '$1'"
    else
        echo "Error: No hits found. Check error log for details."
        echo "$(date) :: No entries for '$1' found. Update the database and try again." >> errorLog.txt
    fi
fi