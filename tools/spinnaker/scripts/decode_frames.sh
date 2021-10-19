#!/bin/sh

if [ -d ${2} ] ; then
    mkdir -p ${3}
    if [ -d ${3} ] ; then
#	cd ${2}
	for file in ${2}/*.raw ; do
	    ./decode_flir_frame -i ${file} -o "${3}/$(basename -s .raw "$file").tiff" -f ${1}
	done
    fi ;
fi ;