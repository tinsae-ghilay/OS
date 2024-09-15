#!/bin/bash

if [[ $# != 0 ]] ; then
	echo "Hello ${1}! This is a bash script for testing."
else
	echo "Hello! This is a bash script for testing, it accepts one argument."
fi
