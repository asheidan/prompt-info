import os

env = Environment(ENV=os.environ)

install_dir = os.path.expanduser("~/bin")


Decider('timestamp-newer')

env.Append(CCFLAGS="-Wall")
env.Append(CCFLAGS="-Wextra")

env.Append(CXXFLAGS="-std=c++98")

# env.Append(CCFLAGS="-g")

# print(env.Dump())
env.Append(LIBPATH="/usr/local/lib")
env.Append(CPPPATH="/usr/local/include")

FEATURES = {
    "git": False,
}

if env.GetOption('clean'):
    FEATURES["git"] = True
else:
    conf = Configure(env)

    if conf.CheckLibWithHeader('git2', 'git2.h', 'c'):
        conf.env.Append(CPPDEFINES="-DLIBGIT2_AVAILABLE")
        FEATURES["git"] = True

        if conf.CheckFunc('git_libgit2_init', '#include <git2/global.h>', 'c'):
            conf.env.Append(CPPDEFINES="-DLIBGIT2_GLOBAL_INIT")
            print("Found git_libgit2_init")

        if conf.CheckFunc('git_threads_init', '#include <git2/threads.h>', 'c'):
            conf.env.Append(CPPDEFINES="-DLIBGIT2_THREADS_INIT")
            print("Found git_threads_init")
    else:
        print("Could not find libgit2, no git-features.")

    env = conf.Finish()

objects = [env.Object('prompt.cpp')]
objects.append(env.Object('AttributedBlock.cpp'))

if FEATURES["git"]:
    objects.append(env.Object('git_status.cpp'))

target = env.Program('prompt-info', objects)
Default(target)

env.Install(dir=install_dir, source=target)

env.Alias('install', [install_dir])
