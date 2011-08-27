env = Environment(tools = ['mingw'])

srcfiles = ['main.c','parser.c','token.c','common.c','interpreter.c','runtime.c']
srcfiles = srcfiles + ['seatest.c']
objfiles = env.Object(srcfiles)

unittests = ARGUMENTS.get('unittest', 0)
if int(unittests):
	env.Append(CCFLAGS = '-DUNITTESTS')
env.Append(CCFLAGS = '-Wall -std=c99')
sharedlib = ARGUMENTS.get('sharedlib', 0)
if int(sharedlib):
	env.Append(CCFLAGS = '-DBETIK_SHAREDLIB')
	env.SharedLibrary(target='betik', source=objfiles)
else:
	env.Program(target='betik', source=objfiles)

