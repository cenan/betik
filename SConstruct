env = Environment(tools = ['mingw'])

srcfiles = ['main.c','parser.c','token.c','common.c','interpreter.c']
srcfiles = srcfiles + ['seatest.c']
objfiles = env.Object(srcfiles)

unittests = ARGUMENTS.get('unittest', 0)
if int(unittests):
	env.Append(CCFLAGS = '-DUNITTESTS')
env.Program(target='betik', source=objfiles)

