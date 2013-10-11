#!/usr/bin/env betik

println("betik repl")
println("type q to quit")

def quit()
	println("type q to quit")
end

def read()
	print ">> "
	return gets()
end

a = read()
while a <> "q"
	eval(a)
	a = read()
end
