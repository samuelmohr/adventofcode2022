/^[0-9]*$/ {
	sum += $1
}
/^\s*$/ {
	if(sum!=0){
		print sum
		sum=0
	}
}
END {
	if(sum!=0){
		print sum
	}
}
