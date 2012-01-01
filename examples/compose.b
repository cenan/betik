#!/usr/bin/env betik

def add(a, b)
	return a+b
end

def double(a)
	return 2*a
end

def compose(f1,f2)
	return def(a,b)
		return f2(f1(a,b))
	end
end

add_double = compose(add, double)
println(add_double(8,7))

