#!/bin/bash
while read line; do
  read fline <&3
  if [ "$line" != "$fline" ];  then
    echo "WRONG! got '$line', expected '$fline'"
  else
    echo "OK! $line"
  fi
done 3<$1
