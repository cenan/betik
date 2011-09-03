#!/usr/bin/env betik

print "betik repl\n"
print "type q to quit\n"

print ">> "
a = gets()
while a <> "q"
	eval(a)
	print ">> "
	a = gets()
end
