// $Header: /nfs/slac/g/glast/ground/cvs/ldfReader/src/EbfDebug.cxx,v 1.1.1.1 2004/04/15 20:02:22 heather Exp $
#include "EbfDebug.h"

int EbfDebug::setDebug(int debugOn) {
    bool old = m_debug;
    m_debug = debugOn;
    return old;
}

int EbfDebug::m_debug;
