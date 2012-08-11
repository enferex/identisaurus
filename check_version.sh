#!/bin/sh

# Check argument passed to this script
if [ $# -ne 1 ]; then
        echo "This script checks the identisaurus inserted version"
        echo "Usage: $0 <binary file>"
        exit -1
fi

# Obtain the filename from the command line passed to this script
file=$1

# Check if the file exists
if [ ! -f $file ]; then
        echo "File '$file' does not exist"
        exit -1
fi

# Use readelf to read the .rodata section of the binary and grep out the
# identisaurus inserted string
readelf -p .rodata $file 2>/dev/null | \
            grep "<identisaurus>" | \
            sed -e 's/.*<identisaurus>//' -e 's/<\/identisaurus>//'
