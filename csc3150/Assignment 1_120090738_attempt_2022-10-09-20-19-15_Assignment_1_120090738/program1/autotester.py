import glob
import os

files = list(glob.glob('*'))

print(files)

programs = filter(lambda x: '.c' not in x, files)
programs = filter(lambda x: 'Makefile' not in x, programs)
programs = filter(lambda x: 'program' not in x, programs)
programs = filter(lambda x: '.py' not in x, programs)
programs = list(programs)
print(programs)
print(len(programs))
sorted(programs)

for idx, program in enumerate(programs):
    print('======{}/{} starts ======'.format(idx+1, len(programs)))
    os.system("./program1 ./{}".format(program))
    print('======{}/{} finished ======'.format(idx+1, len(programs)))
    input()