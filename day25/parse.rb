#!/usr/bin/env ruby

def parse(str)
	return 0 if str==""
	
	number = 0
	factor = 1
	# parse digits
	str.split('').reverse_each do |ch|
		case ch
		when '2'
			number+=2*factor
		when '1'
			number+=factor
		when '0'
			;
		when '-'
			number-=factor
		when '='
			number-=2*factor
		else
			raise "Unknown digit"
		end
		factor*=5
	end
	
	return number
end

STDIN.read.split("\n").each do |a|
	number= parse(a);
	STDOUT.write "#{number}\n"
end
