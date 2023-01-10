#!/usr/bin/env ruby

def parse(number)
	raise "too small number" if number<-2
	return "0" if number==0
	str=""
	
	while number!=0 do
		# print last digit
		dig = number%5
		case dig
		when 2
			str+="2"
		when 1
			str+="1"
		when 0
			str+="0"
		when 4
			str+="-"
		when 3
			str+="="
		end
		dig=(dig+2)%5-2
		number=(number-dig)/5
	end
	
	return str.reverse
end

STDIN.read.split("\n").each do |a|
	representation= parse(a.to_i);
	STDOUT.write "#{representation}\n"
end
