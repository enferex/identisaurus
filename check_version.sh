#!/bin/sh

#******************************************************************************
# check_version.sh: Extracts an Identisaurus inserted string from an object
# file.
#
# Copyright (C) 2012 Matt Davis (enferex)
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, see <http://www.gnu.org/licenses/gpl-2.0.html>
#******************************************************************************


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
