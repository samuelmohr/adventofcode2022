BEGIN {
	print "Searching minimum value above "filter_min
	current_min = -1
}
/^[0-9]*$/ {
	if($1 >= filter_min) {
		if(current_min < 0 || $1<=current_min)
			current_min = $1
	}
}
END {
	print current_min
}
