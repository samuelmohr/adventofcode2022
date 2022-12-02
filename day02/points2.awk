BEGIN {
	sample["A"]=1;
	sample["B"]=2;
	sample["C"]=3;
	sample["X"]=-1;
	sample["Y"]=0;
	sample["Z"]=1;
	# Rock = 1
	# Paper = 2
	# Scissors = 3
}
/^[A-C] [X-Z]$/ {
	score = 0
	# score for shape
	score += (sample[$1]+sample[$2]+2)%3 + 1
	# score for round
	score += sample[$2]*3+3
	print score
}
