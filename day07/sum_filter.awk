BEGIN {
	filter_max = 100000
}
/^[0-9]*$/ {
	if($1 <= filter_max)
		sum += $1
}
/^\s*$/ {
	if(sum!=0){
		print sum
	}
	sum=0
}
END {
	if(sum!=0){
		print sum
	}
}
