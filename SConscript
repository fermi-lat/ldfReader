# -*- python -*-
# $Header: /nfs/slac/g/glast/ground/cvs/ldfReader/SConscript,v 1.13 2009/07/17 13:30:43 glastrm Exp $
# Authors: Heather Kelly <heather@lheapop@gsfc.nasa.gov>, Tracy Usher <usher@slac.stanford.edu>
# Version: ldfReader-07-04-01

Import('baseEnv')
Import('listFiles')
Import('packages')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

libEnv.Tool('ldfReaderLib', depsOnly = 1)
ebf_file = libEnv.SharedObject('src/EBF_fileIn.os','/afs/slac/g/glast/ground/GLAST_EXT/redhat4-i686-32bit/ldf/v06-02-00/gcc34/test/EBF_fileIn.c')
libEnv.AppendUnique(CPPPATH=['/afs/slac/g/glast/ground/GLAST_EXT/redhat4-i686-32bit/ldf/v06-02-00/gcc34/test/'])
ldfReader = libEnv.SharedLibrary('ldfReader', listFiles(['src/iterators/*.cxx',
							 'src/data/*.cxx',
							 'src/*.cxx']) + [ebf_file])
			       

							
if baseEnv['PLATFORM'] != 'win32':
	progEnv.AppendUnique(CPPDEFINES = '_FILE_OFFSET_BITS=64')

progEnv.Tool('ldfReaderLib')
test_ldfReader = progEnv.Program('test_ldfReader', ['src/test/test_ldfReader.cxx'])
test_lsfReader = progEnv.Program('test_lsfReader',['src/test/test_lsfReader.cxx'])
test_socketReader = progEnv.Program('test_socketReader', ['src/test/test_socketReader.cxx'])

progEnv.Tool('registerObjects', package = 'ldfReader',
	     libraries = [ldfReader], testApps = [test_ldfReader,
						  test_lsfReader,
						  test_socketReader],
	                                          includes = listFiles(['ldfReader/*'], recursive=True))




