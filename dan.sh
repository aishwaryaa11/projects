#!/bin/bash

rm -f ./tmp/softdev1.txt
rm -f ./tmp/softdev2.txt

if [ $# -ne 4 ] || [ $1 != "-f" ] || [ $3 != "-f" ];
then
  echo "ERROR: Check command line syntax"
  exit 1
elif [ ! -f "$2" ] || [ ! -f "$4" ];
then
  echo "ERROR: Incorrect filename(s)"
  exit 1
elif [[ $2 != *.wav ]] || [[ $4 != *.wav ]];
then
  echo "ERROR: Incorrect filetype(s)"
  exit 1
else
  larceny --r6rs -program fftransform.sps -- $2 "./tmp/softdev1.txt"
  larceny --r6rs -program fftransform.sps -- $4 "./tmp/softdev2.txt"
  #call java file
  java -jar soundCheck.jar ./tmp/softdev1.txt ./tmp/softdev2.txt
  exit 0
fi