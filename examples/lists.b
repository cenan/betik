#!/usr/bin/env betik

def reverse(list)
	i = 0
	while len(list)/2+1 > i
		tmp = list[len(list)-i-1]
		list[len(list)-i-1] = list[i]
		list[i] = tmp
		i = i + 1
	end
end

x = ["1",2,3,4,5,6,7,8,9,10]
reverse(x)
i = 0
while i < len(x)
	println("list["+i+"] : "+x[i])
	i = i + 1
end

