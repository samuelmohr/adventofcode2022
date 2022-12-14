#!/bin/bash

compare () {
  if [[ $# -ne 2 ]]
  then
    echo "Use compare function with two strings."
    exit 255
  fi
  
  #check empty
  if [[ $1 == "" ]]
  then 
    str2=$2
    echo ${#str2}
    return 0
  fi
  if [[ $2 == "" ]]
  then 
    echo -1
    return 0
  fi
  
  #check for integer
  firstelement=$(echo $1 | sed -E 's|^([0-9]*)(.*)|\1|')
  secondelement=$(echo $2 | sed -E 's|^([0-9]*)(.*)|\1|')
  if [[ $firstelement != "" ]]
  then
    if [[ $secondelement != "" ]]
    then
      let diff=$secondelement-$firstelement
      echo $diff
      return 0
    else
      firstelement="[$1]"
      echo $(compare "[$1]" $2)
      return 0
    fi
  else
    if [[ $secondelement != "" ]]
    then
      echo $(compare $1 "[$2]")
      return 0
    else
      # both are lists, compare all elements
      elements1=$(echo $1 | sed -E 's|^\[(.*)\]$|\1|')
      elements2=$(echo $2 | sed -E 's|^\[(.*)\]$|\1|')
      while [ 1 ]
      do
        el1=$( ./firstelement.rb "$elements1")
        el2=$( ./firstelement.rb "$elements2")
        res=$(compare "$el1" "$el2")
        if [[ $res -ne 0 || $el1 == "" ]]
        then
          echo $res
          return 0
        else
          elements1=$(echo $elements1 | sed -E "s|^.{${#el1}},?||")
          elements2=$(echo $elements2 | sed -E "s|^.{${#el2}},?||")
        fi
      done
    fi
  fi
}

# quicksorts positional arguments
# return is in array qsort_ret
qsort() {
   local pivot i smaller=() larger=()
   qsort_ret=()
   (($#==0)) && return 0
   pivot=$1
   shift
   for i; do
      # This sorts strings lexicographically.
      comp_value=$(compare $i $pivot)
      if [[ $comp_value -ge 0 ]]; then
         smaller+=( "$i" )
      else
         larger+=( "$i" )
      fi
   done
   qsort "${smaller[@]}"
   smaller=( "${qsort_ret[@]}" )
   qsort "${larger[@]}"
   larger=( "${qsort_ret[@]}" )
   qsort_ret=( "${smaller[@]}" "$pivot" "${larger[@]}" )
}

arr=()
while IFS= read -r line
do
  if [[ $line != "" ]]
  then
    arr+=("$line")
  fi
done

qsort "${arr[@]}"
printf "%s\n" "${qsort_ret[@]}"
