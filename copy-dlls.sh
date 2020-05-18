#!/bin/bash

# Adapted from: https://blog.rubenwardy.com/2018/05/07/mingw-copy-dlls/

BINDIR="."
EXE="$BINDIR/*.exe"
PREFIX="x86_64-w64-mingw32"

# Paths to search
paths=("/mingw32/bin")

# Returns 1 if a file was copied, 0 otherwise
function findAndCopyDLL() {
	DEST="$BINDIR/$1"
	if [ -f $DEST ]; then
		return 0
		# Skip, file already in place
	fi
	
    for i in "${paths[@]}"
    do
		FILE="$i/$1"
		if [ -f $FILE ]; then
		   
		   cp $FILE $BINDIR
		   echo "Found $1 in $i"
		   
		   return 1
	    fi
    done
    return 0
}

function copyForOBJ() {
	# Set nullglob option to handle the case where *.dll doesn't match anything
	shopt -s nullglob 
	dlls=`objdump -p $1 $BINDIR/*.dll | grep 'DLL Name:' | sed -e "s/\t*DLL Name: //g"`
	dlls_copied=0
    while read -r filename; do
		findAndCopyDLL $filename
        dlls_copied=$(($dlls_copied+$?))
	done <<< "$dlls"
	return $dlls_copied
}

# Start checking and copying dlls
copyForOBJ $EXE;
# Repeat until no more DLLS are added
dlls_copied=$?
while [ $dlls_copied -gt 0 ]
do
	copyForOBJ $EXE;
	dlls_copied=$?
done
