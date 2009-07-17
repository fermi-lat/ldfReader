// $Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/EbfDebug.cxx,v 1.2 2009/03/17 13:21:46 heather Exp $
#include "EbfDebug.h"

int EbfDebug::setDebug(int debugOn) {
    int old = m_debug;
    m_debug = debugOn;
    return old;
}

int EbfDebug::m_debug;
