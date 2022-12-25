#!/usr/bin/env ruby

def compare(str1,str2)
#	STDOUT.write "compare #{str1} and #{str2}\n"
	return str2.length if str1==""
	return -1 if str2==""
	
	# check for integers
	number_re= /^([0-9]*)(.*)/m
	number_subst = '\1'
	firstelement = str1.to_s.gsub(number_re,number_subst)
	secondelement = str2.to_s.gsub(number_re,number_subst)

	unless firstelement=="" then
		return secondelement.to_i - firstelement.to_i unless secondelement==""
		return compare("[#{str1}]",str2)
	else
		return compare(str1,"[#{str2}]") unless secondelement==""
		# both are lists
		str1=str1[1..-2]
		str2=str2[1..-2]
		re = /^(?<return>(?<element>\d+||(?<elementlist>\[\g<element>(,\g<element>)*\])))(,\g<element>)*$/m
		subst = '\k<return>'
		while true do
			el1=str1.to_s.gsub(re, subst)
			el2=str2.to_s.gsub(re, subst)
			res=compare(el1,el2)
			return res if res!=0 or el1==""
			str1=str1[el1.length+1..-1]
			str2=str2[el2.length+1..-1]
		end
	end
end

ary = Array.new 
STDIN.read.split("\n").each do |a|
	ary.push(a) unless a==""
end

ary.push("[[2]]")
ary.push("[[6]]")

ary=ary.sort { |a,b| -compare(a,b) }

puts ary
