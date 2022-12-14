#!/usr/bin/env ruby
re = /^(?<return>(?<element>\d+||(?<elementlist>\[\g<element>(,\g<element>)*\])))(,\g<element>)*$/m
str = "#{ARGV[0]}"
subst = '\k<return>'

result = str.gsub(re, subst)

# Print the result of the substitution
puts result
