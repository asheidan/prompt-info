import os

env = Environment(ENV=os.environ)

Decider('timestamp-newer')

env.Append(CCFLAGS="-Wall")
env.Append(CCFLAGS="-Wextra")

env.Append(CXXFLAGS="-std=c++98")

env.Append(CCFLAGS="-g")

#print(env.Dump())
env.Append(LIBPATH="/usr/local/lib")
env.Append(CPPPATH="/usr/local/include")

if not env.GetOption('clean'):
    conf = Configure(env)

    if not conf.CheckLibWithHeader('git2', 'git2.h', 'c'):
        print("Could not find libgit2, no git-features.")
        #Exit(1)
    else:
        conf.env.Append(CPPDEFINES="-DLIBGIT2_AVAILABLE")

    env = conf.Finish()

objects = env.Object('prompt.cpp')

#git_status = env.Program('git_status.cpp')

target = env.Program('prompt-info', objects)

