BEGIN {
	sample["A"]=1;
	sample["B"]=2;
	sample["C"]=3;
	sample["X"]=1;
	sample["Y"]=2;
	sample["Z"]=3;
	# Rock = 1
	# Paper = 2
	# Scissors = 3
}
/^[A-C] [X-Z]$/ {
	score = 0
	# score for shape
	score += sample[$2]
	# score for draw
	if(sample[$1] == sample[$2])
		score += 3
	# score for win
	if((6+sample[$2]-sample[$1]) % 3 == 1)
		score += 6
	print score
}
