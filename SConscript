# -*- python -*-
# $Header: /nfs/slac/g/glast/ground/cvs/ldfReader/SConscript,v 1.14.2.4 2011/03/30 02:58:24 heather Exp $
# Authors: Heather Kelly <heather@lheapop@gsfc.nasa.gov>, Tracy Usher <usher@slac.stanford.edu>
# Version: ldfReader-07-04-06

import os

Import('baseEnv')
Import('listFiles')
Import('packages')

progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()


libEnv.Tool('addLinkDeps', package='ldfReader', toBuild='shared')
#ebf_file = libEnv.SharedObject('src/EBF_fileIn.os',
#                              '/afs/slac/g/glast/ground/GLAST_EXT/redhat4-i686-32bit/ldf/v06-02-00/gcc34/test/EBF_fileIn.c')

if baseEnv['PLATFORM'] != 'win32':
	progEnv.AppendUnique(CPPDEFINES = '_FILE_OFFSET_BITS=64')
else:
        progEnv.AppendUnique(CPPDEFINES = 'WIN32')
        progEnv.AppendUnique(CPPDEFINES = '_WIN32_LDF_STATIC')
        libEnv.AppendUnique(CPPDEFINES = 'WIN32')
        libEnv.AppendUnique(CPPDEFINES = '_WIN32_LDF_STATIC')

ebf_file = libEnv.SharedObject('src/EFF_fileIn.os',
                               os.path.join(baseEnv['ldfTestPath'], 'EBF_fileIn.c'))
##libEnv.AppendUnique(CPPPATH=[baseEnv['ldfTestPath']])
ldfReader = libEnv.SharedLibrary('ldfReader',
                                 listFiles(['src/iterators/*.cxx',
                                            'src/data/*.cxx',
                                            'src/*.cxx']) + [ebf_file])
							

progEnv.Tool('ldfReaderLib')
test_ldfReader = progEnv.Program('test_ldfReader',
                                 ['src/test/test_ldfReader.cxx'])
test_lsfReader = progEnv.Program('test_lsfReader',
                                 ['src/test/test_lsfReader.cxx'])
test_socketReader = progEnv.Program('test_socketReader',
                                    ['src/test/test_socketReader.cxx'])

progEnv.Tool('registerTargets', package = 'ldfReader',
	     libraryCxts = [[ldfReader, libEnv]],
             testApps = [[test_ldfReader, progEnv], [test_lsfReader,progEnv],
                         [test_socketReader, progEnv]],
             includes = listFiles(['ldfReader/*'], recursive=True))




