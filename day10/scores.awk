BEGIN {
	cycle = 0
	X=1
}
/^noop$/ {
	cycle++
	if(cycle%40 == 20) 
		print cycle*X
}
/^addx [+|-]?([0-9])+$/ {
	cycle++
	if(cycle%40 == 20) 
		print cycle*X
	cycle++
	if(cycle%40 == 20) 
		print cycle*X
	X += $2
}
END {
	if(cycle%40 == 20) 
		print cycle*X
}

