BEGIN {
	cycle = 0
	X=1
}
function abs(v) {return v < 0 ? -v : v}
function draw() {
	pos = cycle%40
	if(abs(pos-X)<=1)
		printf "%s", "#"
	else
		printf "%s", "."
	if(pos ==39)
		print ""
}
/^noop$/ {
	draw()
	cycle++
}
/^addx [+|-]?([0-9])+$/ {
	draw()
	cycle++
	draw()
	cycle++
	X += $2
}

