env = Environment()

env.Append(CCFLAGS="-Wall")
env.Append(CCFLAGS="-Wextra")

env.Append(CXXFLAGS="-std=c++98")

env.Append(CCFLAGS="-g")

#print(env.Dump())

if not env.GetOption('clean'):
    conf = Configure(env)

    if not conf.CheckLib('git2'):
        print("Could not find libgit2")
        #Exit(1)

    env = conf.Finish()

objects = env.Object('prompt.cpp')

#git_status = env.Program('git_status.cpp')

target = env.Program('prompt-info', objects)

