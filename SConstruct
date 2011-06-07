env = Environment(tools = ['mingw'])

srcfiles = ['main.c','parser.c','token.c','common.c']
srcfiles = srcfiles + ['seatest.c']
objfiles = env.Object(srcfiles)
env.Append()
env.Program(target='betik', source=objfiles)

