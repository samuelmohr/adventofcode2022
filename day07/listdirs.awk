BEGIN {
	depth = -1
}
/^\$ cd (\w|\/)+$/ {
	depth++
	summing[depth]=0
}
/^([0-9])+ .*$/ {
	summing[depth] += $1
}
/^\$ cd \.\.$/ {
	print summing[depth]
	summing[depth-1] += summing[depth]
	depth--
}
END {
	for(;depth>=0;depth--) {
		print summing[depth]
		if(depth != 0)
			summing[depth-1] += summing[depth]
	}
}

