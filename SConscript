# -*- python -*-
# $Header: /nfs/slac/g/glast/ground/cvs/ldfReader/SConscript,v 1.2 2008/11/10 16:19:40 ecephas Exp $
# Authors: Heather Kelly <heather@lheapop@gsfc.nasa.gov>, Tracy Usher <usher@slac.stanford.edu>
# Version: ldfReader-07-00-00
Import('baseEnv')
Import('listFiles')
Import('packages')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

libEnv.Tool('ldfReaderLib', depsOnly = 1)
ldfReader = libEnv.SharedLibrary('ldfReader', listFiles(['src/*.cxx','src/iterators/*.cxx','src/data/*.cxx']))

if baseEnv['PLATFORM'] != 'win32':
	progEnv.AppendUnique(CPPDEFINES = '_FILE_OFFSET_BITS=64')

progEnv.Tool('ldfReaderLib')

test_ldfReader = progEnv.Program('test_ldfReader', ['src/test/test_ldfReader.cxx'])
test_lsfReader = progEnv.Program('test_lsfReader',['src/test/test_lsfReader.cxx'])
test_socketReader = progEnv.Program('test_socketReader', ['src/test/test_socketReader.cxx'])

progEnv.Tool('registerObjects', package = 'ldfReader', libraries = [ldfReader], 
	testApps = [test_ldfReader,test_lsfReader,test_socketReader],
	includes = listFiles(['ldfReader/*'], recursive = 1))
