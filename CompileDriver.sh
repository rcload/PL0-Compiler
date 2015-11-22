#!/usr/bin/env bash

LFLAG="0"
AFLAG="0"
VFLAG="0"

while getopts "lav" opt; 
do
  case $opt in
    l)
      LFLAG="1"
      ;;
    a)
      AFLAG="1"
      ;;
    v)
      VFLAG="1"
      ;;
    /?)
      echo "Invalid option: -$OPTARG" 
      exit 1
      ;;
    
  esac
done

gcc scanner.c -o scanner
./scanner

gcc parser.c -o parser

if [ "$LFLAG" = "1" ] && [ "$AFLAG" = "1" ]; then
  ./parser -l -a
elif [ "$LFLAG" = "1" ]; then
	./parser -l
elif [ "$AFLAG" = "1" ]; then
	./parser -a
else
  ./parser
fi

gcc vm.c -o vm

if [ "$VFLAG" = "1" ]; then
	 ./vm -v
else
	 ./vm
fi
