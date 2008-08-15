# $Header$
def generate(env, **kw):
    if not kw.get('depsOnly', 0):
        env.Tool('addLibrary', library = 'ldfReader')

def exists(env):
    return 1;
